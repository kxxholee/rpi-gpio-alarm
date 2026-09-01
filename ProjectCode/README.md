# 최종 구현 — 재실 인원 임계값 감시

기말 프로젝트의 제출 구현이다. 시스템 개요와 전체 빌드·실행 절차는 최상위
[README](../README.md)에 기술하였으며, 본 문서는 이 디렉터리의 구성만 설명한다.

## 구성

| 경로 | 내용 |
|---|---|
| `main.cpp` | 추론 루프. 영상 입력, TFLite 호출, 검출 계수, 디바이스 제어를 수행한다. |
| `CMakeLists.txt` | 응용 빌드 정의. OpenCV와 TensorFlow Lite를 링크한다. |
| `Drivers/` | GPIO 문자 디바이스 드라이버 3종과 Kbuild `Makefile` |
| `model/model.tflite` | SSD MobileNet 검출 모델 (COCO 90 클래스, 양자화 uint8) |
| `installScript/` | TensorFlow Lite 2.6.0 의존성 설치 스크립트 (수정본) |
| `tensorflow/` | TFLite 헤더 및 정적 라이브러리 배치 위치 (저장소 미포함) |

## 빌드

```bash
# 1. 커널 모듈
cd Drivers && make
sudo insmod buttonDriver.ko
sudo insmod segmentDriver.ko
sudo insmod ledDriver.ko
cd ..

# 2. 응용 프로그램
mkdir -p build && cd build
cmake .. && make

# 3. 실행
sudo ./TermProject17 [영상_파일_경로]
```

## 환경 구성 시 유의사항

- TensorFlow Lite는 `installScript/modified_tensorflow_2.6.0_install.sh`로 설치한다.
  과목 제공 원본 스크립트는 armv7l과 호환되지 않는 FlatBuffers 버전을 내려받는다.
- `main.cpp`의 `pwd` 상수와 `CMakeLists.txt`의 TFLite 경로가 개발 당시 보드의 절대
  경로로 고정되어 있다. 다른 환경에서는 두 파일을 수정해야 한다.

## 참고

- TensorFlow Lite 빌드 가이드 (Raspberry Pi) — <https://www.tensorflow.org/lite/guide/build_rpi>
