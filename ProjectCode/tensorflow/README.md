# TensorFlow Lite 헤더 및 라이브러리 배치 위치

본 디렉터리는 비어 있다. `main.cpp`가 포함하는 아래 헤더와, 링크 대상인
`libtensorflow-lite.a`는 용량 문제로 저장소에 포함하지 않으며 보드에서 직접 빌드하여
이 위치에 배치한다.

```cpp
#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/model.h>
```

## 구성 절차

```bash
../installScript/modified_tensorflow_2.6.0_install.sh
```

이 스크립트는 빌드 도구, TensorFlow Lite 2.6.0 헤더, 그리고 armv7l(32-bit)과 호환되는
FlatBuffers v2.0.0 헤더를 내려받는다. 과목에서 제공된 원본 스크립트는 호환되지 않는
FlatBuffers 버전을 내려받아 빌드가 실패하므로 반드시 이 수정본을 사용한다.

## 빌드 후 예상 구조

```
ProjectCode/tensorflow/
├── tensorflow/lite/…                                  헤더
└── tensorflow/lite/tools/make/gen/rpi_armv7l/lib/
    └── libtensorflow-lite.a                           정적 라이브러리
```

`CMakeLists.txt`의 `TFLITE_INCLUDE_DIR`과 `TFLITE_LIB_DIR`이 위 두 경로를 가리키도록
설정되어 있다. 두 값은 개발 당시의 절대 경로로 고정되어 있으므로 다른 환경에서는 직접
수정해야 한다 (최상위 README 8.3절 참조).

## 참고

- TensorFlow Lite 빌드 가이드 (Raspberry Pi) — <https://www.tensorflow.org/lite/guide/build_rpi>
