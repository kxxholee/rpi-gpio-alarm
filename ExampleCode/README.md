# 주차별 실습 코드

임베디드시스템(2023) 강의의 주차별 실습 및 과제 코드이다. 최종 프로젝트 구현은
[`ProjectCode/`](../ProjectCode)에 있다.

## 구성

| 주차 | 주제 | 내용 |
|---|---|---|
| [week07](week07) | `mmap` 기반 GPIO 직접 제어 | 물리 메모리를 사용자 공간에 매핑하여 레지스터를 직접 조작한다. `led_mmap.c`, `button_mmap.c`, `keyboard_mmap.c` |
| [week10](week10) | 문자 디바이스 드라이버 | 커널 모듈로 GPIO를 제어한다. 7-세그먼트 드라이버(`seg_driver.c`)와 응용 예제, 과제(`Homework/`)로 버튼 연동 드라이버를 구현한다. |
| [week11](week11) | OpenCV 영상 입출력 | 카메라 캡처, 흑백 변환, 파일 기록. 예제 코드의 `VideoWriter` 경로 오류 분석은 [week11/README.md](week11/README.md)에 기술하였다. |
| [week12](week12) | TensorFlow Lite 객체 검출 | COCO SSD MobileNet 검출 모델(`detect.tflite`)과 라벨맵, 설치 스크립트 |

week10의 드라이버 구현과 week11–12의 OpenCV·TFLite 연동이 최종 프로젝트의 직접적인 기반이
되었다.

## 환경 구성

| 대상 | 스크립트 |
|---|---|
| OpenCV 4.5.1 | [`week11/opencv_example/install_cv4.5.1_rasbian.sh`](week11/opencv_example/install_cv4.5.1_rasbian.sh) |
| TensorFlow Lite 2.6.0 | [`week12/install_tensorflowlite2.6.0_rasbian.sh`](week12/install_tensorflowlite2.6.0_rasbian.sh) |

최종 프로젝트에서는 FlatBuffers 버전 호환 문제로 TFLite 설치 스크립트의 수정본
([`ProjectCode/installScript/`](../ProjectCode/installScript))을 사용한다.
