# 재실 인원 임계값 감시 시스템

**건국대학교 전기전자공학부 · 임베디드시스템 (2023) 기말 프로젝트 — 17조**

| | |
|---|---|
| 담당교수 | 조용범 |
| 조원 | 이관호 (202110965), 김세연 (201810804), 김진구 (202014122) |
| 대상 플랫폼 | Raspberry Pi 4 Model B |

---

## 1. 개요

Raspberry Pi 4B 상에서 TensorFlow Lite 객체 검출 모델과 직접 구현한 Linux 문자 디바이스
드라이버를 결합하여, 영상에 포착된 인원 수를 실시간으로 계수하고 사용자가 지정한 임계값과
비교하여 그 결과를 GPIO 주변장치로 표시하는 임베디드 응용을 구현하였다.

본 프로젝트의 목적은 단순한 객체 검출의 구현이 아니라, **사용자 공간의 추론 결과가
`read(2)` / `write(2)` 시스템 콜을 거쳐 커널 공간의 드라이버를 통해 하드웨어 주변장치에
도달하는 전 경로를 직접 구성**하는 데 있다. 이에 따라 응용 계층(OpenCV, TensorFlow Lite)과
커널 계층(GPIO 문자 디바이스 드라이버 3종)을 모두 작성하였다.

## 2. 시스템 구성

```
                  ┌──────────────────────── 사용자 공간 ────────────────────────┐
                  │                                                             │
   video file ───▶│  OpenCV VideoCapture ──▶ 전처리 (640px 정규화 → 300×300)    │
                  │                                    │                        │
                  │                                    ▼                        │
                  │                     TFLite Interpreter (SSD MobileNet)      │
                  │                                    │                        │
                  │                       person 클래스 검출 계수 → N           │
                  │                                    │                        │
                  │       ┌────────────────────────────┼──────────────┐         │
                  │       │ read()                     │ write()      │ write() │
                  └───────┼────────────────────────────┼──────────────┼─────────┘
                          │                            │              │
                  ┌───────┼────────────────────────────┼──────────────┼─────────┐
                  │       ▼                            ▼              ▼         │
                  │  /dev/my_button           /dev/my_segment   /dev/my_led     │
                  │  buttonDriver.ko          segmentDriver.ko  ledDriver.ko    │
                  └───────┼────────────────────────────┼──────────────┼─────────┘
                          │            커널 공간       │              │
                  ┌───────┼────────────────────────────┼──────────────┼─────────┐
                  │  GPIO 5, 6                  GPIO ×12         GPIO 22        │
                  │  임계값 M 증감              4-digit 7-seg     상태 LED      │
                  │                             (M 표시)         (N ≥ M 시 점등) │
                  └──────────────────────── 하드웨어 ──────────────────────────┘
```

프레임당 동작 절차는 다음과 같다.

1. `cv::VideoCapture`로 프레임을 획득하고 종횡비를 유지한 채 가로 640 px로 정규화한다.
2. 추론 입력 규격에 맞추어 300 × 300 × 3 (uint8)로 재조정한다.
3. TFLite 인터프리터를 호출하여 검출 상자, 클래스, 신뢰도를 얻는다.
4. 클래스가 `person`인 검출만을 계수하여 현재 인원 수 **N**을 구한다.
5. `/dev/my_button`을 읽어 버튼 입력에 따라 임계값 **M**을 증감한다 (0 – 9999 순환).
6. `/dev/my_segment`에 **M**을 기록하여 7-세그먼트에 표시한다.
7. **N ≥ M**이면 `/dev/my_led`에 `'u'`를, 아니면 `'d'`를 기록하여 LED를 제어한다.

## 3. 하드웨어 인터페이스

| 주변장치 | 디바이스 노드 | GPIO (BCM) | 방향 | 전송 단위 | 규약 |
|---|---|---|---|---|---|
| 버튼 ×2 | `/dev/my_button` | 5, 6 | 입력 | `unsigned char` | `'u'` 증가, `'d'` 감소, `'0'` 입력 없음 |
| 4-digit 7-세그먼트 | `/dev/my_segment` | 2, 3, 4, 17, 21, 20, 16, 12, 7, 8, 25, 24 | 출력 | `unsigned short` | 상위 8비트 세그먼트 패턴, 하위 4비트 자릿수 선택 |
| 상태 LED | `/dev/my_led` | 22 | 출력 | `unsigned char` | `'u'` 점등, `'d'` 소등 |

세그먼트 드라이버는 12개의 GPIO를 점유한다. 8개는 세그먼트 선(a–g 및 dp), 4개는 자릿수
선택선에 대응하며, 응용에서는 자릿수 선택 비트를 순차적으로 갱신하는 시분할 구동 방식으로
표시한다.

## 4. 소프트웨어 스택

| 계층 | 구성 요소 | 비고 |
|---|---|---|
| 운영체제 | Raspberry Pi OS (Raspbian) | 커널 6.0.19-v7l+, armv7l (32-bit) |
| 커널 모듈 | `buttonDriver`, `segmentDriver`, `ledDriver` | 문자 디바이스, `linux/gpio.h` 기반 |
| 추론 | TensorFlow Lite 2.6.0 (C++) | SSD MobileNet, COCO 90 클래스, 양자화 uint8 |
| 영상 처리 | OpenCV 4.5.1 | 캡처, 크기 조정, 검출 결과 시각화 |
| 빌드 | CMake ≥ 3.0 / Kbuild | 응용은 CMake, 모듈은 커널 빌드 시스템 |

## 5. 저장소 구조

```
├── ProjectCode/                최종 제출 구현
│   ├── main.cpp                추론 루프 및 디바이스 제어
│   ├── CMakeLists.txt          응용 빌드 정의
│   ├── Drivers/                GPIO 문자 디바이스 드라이버 3종 + Kbuild Makefile
│   ├── model/model.tflite      SSD MobileNet 검출 모델
│   ├── installScript/          TFLite 2.6.0 의존성 설치 스크립트 (수정본)
│   └── tensorflow/             TFLite 헤더·라이브러리 배치 위치 (저장소 미포함)
├── EyeStateClassifier/         1차 접근의 학습 파이프라인 — 폐기 (7.1절)
├── ExampleCode/                주차별 실습 코드
│   ├── week07/                 mmap 기반 GPIO 직접 제어
│   ├── week10/                 문자 디바이스 드라이버, 7-세그먼트
│   ├── week11/                 OpenCV 카메라 입출력
│   └── week12/                 TFLite 객체 검출 예제
├── Documents/                  주차별 보고서 및 학습용 노트북
└── Memo/                       PC–보드 간 연결 방식에 대한 조사 기록
```

## 6. 빌드 및 실행

### 6.1 사전 요구사항

보드에서 다음을 준비한다.

- 커널 소스 트리 (`Drivers/Makefile`의 `KDIR`이 `~/working/kernel`을 가리킨다)
- OpenCV 4.5.1 — `ExampleCode/week11/opencv_example/install_cv4.5.1_rasbian.sh`
- TensorFlow Lite 2.6.0 — `ProjectCode/installScript/modified_tensorflow_2.6.0_install.sh`

TFLite는 반드시 위 수정본 스크립트로 설치한다. 사유는 8.1절에 기술하였다.

### 6.2 커널 모듈 빌드 및 적재

```bash
cd ProjectCode/Drivers
make
sudo insmod buttonDriver.ko
sudo insmod segmentDriver.ko
sudo insmod ledDriver.ko
```

적재 결과는 `dmesg`와 디바이스 노드 생성 여부로 확인한다.

```bash
dmesg | tail
ls -l /dev/my_button /dev/my_segment /dev/my_led
```

### 6.3 응용 프로그램 빌드

```bash
cd ProjectCode
mkdir -p build && cd build
cmake ..
make
```

### 6.4 실행

```bash
sudo ./TermProject17 [영상_파일_경로]
```

인자를 생략하면 `videos/video3.mp4`를 사용한다. 디바이스 노드 접근을 위해 `sudo`가
필요하다. 실행 중 아무 키나 누르면 종료한다.

### 6.5 정리

```bash
sudo rmmod ledDriver segmentDriver buttonDriver
```

## 7. 설계 변경 이력

### 7.1 1차 접근 — 눈 상태 분류 기반 졸음 감지 (폐기)

초기 설계는 운전자 졸음 감지를 목표로 하였다. dlib의 68점 얼굴 랜드마크 검출기로 눈 영역을
추출한 뒤, 26 × 34 흑백 영상을 입력받아 눈의 개폐 여부를 판별하는 이진 분류 CNN을 직접
학습시키는 구성이었다. 해당 파이프라인은 `EyeStateClassifier/`에 남아 있다.

| 파일 | 역할 |
|---|---|
| `model.py` | Conv(32) – Conv(64) – Conv(128) – FC(512) – FC(1, sigmoid) 구성의 분류망 |
| `data_loader.py` | `tf.data` 기반 눈 영상 데이터셋 로더 |
| `train.py` | 학습 루프 |
| `validation.py` | 검증 |
| `converter.py` | 학습 가중치를 `.tflite`로 변환 |

이 접근은 다음의 이유로 폐기하였다.

1. 얼굴 랜드마크 검출기(약 69 MB)와 분류망을 함께 적재할 경우 보드의 실시간 처리 성능이
   확보되지 않았다.
2. 자체 학습망의 일부 연산이 TFLite 변환 과정에서 호환되지 않아 추가 수정이 필요하였다
   (`model.py` 주석 참조).
3. 제한된 기간 내에 학습 데이터의 품질을 확보하기 어려웠다.

학습 데이터셋과 가중치 파일은 저장소 용량 관리를 위해 이력에서 제거하였다.

### 7.2 2차 접근 — 사전 학습 검출 모델 (채택)

자체 학습망 대신 COCO로 사전 학습된 SSD MobileNet 양자화 모델을 채택하고, 문제를 "졸음
판별"에서 "인원 계수 및 임계값 감시"로 재정의하였다. 이로써 학습 부담을 제거하고 커널
드라이버와 사용자 공간 응용 간의 연동이라는 본 과목의 핵심 주제에 집중할 수 있었다.

## 8. 알려진 문제 및 제약

### 8.1 FlatBuffers 버전 비호환 (해결)

과목에서 제공된 TFLite 설치 스크립트가 내려받는 FlatBuffers 헤더가 armv7l(32-bit)
아키텍처와 호환되지 않아 빌드가 실패하였다. FlatBuffers v2.0.0을 명시적으로 내려받도록
스크립트를 수정하여 해결하였다.
→ `ProjectCode/installScript/modified_tensorflow_2.6.0_install.sh`

### 8.2 OpenCV `VideoWriter` 경로 오류 (해결)

예제 코드에서 `VideoWriter` 객체 생성 시 출력 파일 경로 처리에 문제가 있어 파일이 생성되지
않았다. 원인 분석과 해결 과정은 `ExampleCode/week11/README.md`에 기술하였다.

### 8.3 절대 경로 하드코딩 (미해결)

`main.cpp`의 `pwd` 상수와 `CMakeLists.txt`의 `TFLITE_INCLUDE_DIR`,
`TFLITE_LIB_DIR`이 개발 당시 보드의 절대 경로(`/home/pi/working/ProjectCode/CTesting/`)로
고정되어 있다. 다른 환경에서 빌드·실행하려면 두 파일을 직접 수정해야 한다.

### 8.4 검출 개수 순회 오류 (미해결)

`main.cpp`의 검출 결과 순회는 다음과 같이 작성되어 있다.

```cpp
const auto size = interpreter->output_tensor(0)->dims->size;
for (int i = 0; i < size; ++i) { ... }
```

`dims->size`는 출력 텐서의 **차원 수**이며 검출 개수가 아니다. 검출 상자 텐서의 형상이
`[1, 10, 4]`이므로 이 값은 3이 되어, 모델이 반환하는 검출 후보 중 앞의 3개만 검사된다.
실제 검출 개수는 네 번째 출력 텐서(`numDetect`)에 담겨 있으나 현재 사용되지 않는다.
따라서 화면에 4명 이상이 존재하더라도 계수는 3을 초과하지 않는다.

### 8.5 7-세그먼트 상위 자릿수 미구동 (미해결)

`displaySegment()`는 4자리 표시를 전제로 작성되었으나, 상위 3자리에 해당하는
`DisplayBuff[0]`–`DisplayBuff[2]`가 0으로 고정되어 있어 실제로는 1의 자리만 표시된다.
임계값 **M**은 9999까지 증가할 수 있으므로 10 이상의 값은 올바르게 표시되지 않는다.

### 8.6 저장소 미포함 자산

다음은 저장소에 포함되어 있지 않으며 별도로 준비해야 한다.

- `ProjectCode/tensorflow/` — TFLite 헤더 및 정적 라이브러리 (6.1절의 설치 스크립트로 구성)
- `videos/` — 입력 영상
- `EyeStateClassifier/dataset/`, `EyeStateClassifier/weights/` — 1차 접근의 학습 데이터 및 가중치

## 9. 참고 자료

- TensorFlow Lite 빌드 가이드 (Raspberry Pi) — <https://www.tensorflow.org/lite/guide/build_rpi>
- E. Juras, *TensorFlow Lite Object Detection on Android and Raspberry Pi* —
  <https://github.com/EdjeElectronics/TensorFlow-Lite-Object-Detection-on-Android-and-Raspberry-Pi>
  (학습 절차는 `Documents/week12/Train_TFLite2_Object_Detction_Model.ipynb` 참조)
- COCO SSD MobileNet v1 (quantized, 2018-06-29) — 검출 모델 및 `labelmap.txt` 출처
