import os
# 0 = all messages are logged (default behavior)
# 1 = INFO messages are not printed
# 2 = INFO and WARNING messages are not printed
# 3 = INFO, WARNING, and ERROR messages are not printed
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3' # 하고싶으면 하는거임
import tensorflow as tf
from model import Net
import numpy as np

# 모델 로드
model = Net()

# # 더미 입력 데이터 넣기
# _ = model(np.random.random((1, 26, 34, 1)).astype(np.float32))

model.load_weights('weights/classifier_weights.h5')

# TensorFlow Lite 모델로 변환
converter = tf.lite.TFLiteConverter.from_keras_model(model)
tflite_model = converter.convert()

# .tflite 파일로 저장
with open('weights/model.tflite', 'wb') as f:
    f.write(tflite_model)

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '0'