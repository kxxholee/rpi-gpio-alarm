# 눈 상태 분류기 (1차 접근 · 폐기)

본 디렉터리는 최종 제출물이 아니다. 기말 프로젝트의 **1차 설계에서 사용한 학습
파이프라인**이며, 설계 변경의 근거를 남기기 위해 보존한다. 폐기 사유를 포함한 전체 경위는
저장소 최상위 [README의 7.1절](../README.md#71-1차-접근--눈-상태-분류-기반-졸음-감지-폐기)에
기술하였다.

## 목적

졸음 감지를 목표로, dlib의 68점 얼굴 랜드마크 검출기로 추출한 눈 영역(26 × 34 흑백 영상)을
입력받아 눈의 개폐 여부를 판별하는 이진 분류기를 학습시킨다.

## 출처

외부에 공개된 PyTorch 구현을 TensorFlow Lite로 변환하기 위해 Keras로 재구현한 것이다.
입력 규격(26 × 34 흑백)이 참고 구현과 동일하며, `model.py`의 주석에도 참고 사실이 기록되어
있다. 참고 구현 자체는 한때 `ProjectCode/sleep_detect/`에 포함되어 있었으나 현재는 저장소에
없다.

> ys-cs17, *Sleepy eyes detector* — <https://ys-cs17.tistory.com/24>

## 구성

| 파일 | 역할 |
|---|---|
| `model.py` | Conv(32) – Conv(64) – Conv(128) – FC(512) – FC(1, sigmoid) 분류망 정의 |
| `data_loader.py` | `tf.data.Dataset` 기반 눈 영상 데이터셋 로더 |
| `train.py` | 학습 루프 (Adam, 배치 32) |
| `validation.py` | 검증 |
| `converter.py` | Keras 가중치를 TensorFlow Lite 모델로 변환 |

## 실행

학습 데이터셋(`dataset/`)과 가중치(`weights/`)는 용량 관리를 위해 저장소 이력에서
제거하였으므로, 아래 절차는 해당 자산을 직접 준비한 경우에만 재현할 수 있다.

```bash
python train.py       # dataset/x_train.npy, y_train.npy 필요
python validation.py  # dataset/x_val.npy, y_val.npy 필요
python converter.py   # weights/classifier_weights.h5 → weights/model.tflite
```

기대 디렉터리 구조는 다음과 같다.

```
EyeStateClassifier/
├── dataset/
│   ├── x_train.npy   (N, 26, 34, 1) float32
│   ├── y_train.npy   (N,) float32 — 0: 감김, 1: 뜸
│   ├── x_val.npy
│   └── y_val.npy
└── weights/
    ├── classifier_weights.h5
    └── model.tflite
```

## 알려진 제약

`model.py`의 주석에 기록된 바와 같이, 이 분류망의 일부 연산은 TensorFlow Lite 변환 과정에서
호환되지 않아 추가 수정이 필요한 상태로 남아 있다. 최종 구현은 이 경로 대신 COCO로 사전
학습된 SSD MobileNet 검출 모델을 채택하였다.
