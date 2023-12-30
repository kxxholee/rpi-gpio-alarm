import os
# 0 = all messages are logged (default behavior)
# 1 = INFO messages are not printed
# 2 = INFO and WARNING messages are not printed
# 3 = INFO, WARNING, and ERROR messages are not printed
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3' 

import tensorflow as tf
import numpy as np
from model import Net
from data_loader import EyesDataset

# 현재 로그 수준 저장 및 수정
# DEBUG < INFO < WARNING < ERROR < CRITICAL
tf.compat.v1.logging.set_verbosity(tf.compat.v1.logging.ERROR)

# 데이터 로드
x_train = np.load('./dataset/x_train.npy').astype(np.float32)
y_train = np.load('./dataset/y_train.npy').astype(np.float32)

# x_train과 y_train의 차원과 크기 확인
print("x_train shape:", x_train.shape)
print("y_train shape:", y_train.shape)

# x_train과 y_train의 샘플 수 확인
print("Number of samples in x_train:", len(x_train))
print("Number of samples in y_train:", len(y_train))

# 사용 가능한 GPU 목록 출력
print(f"Available GPUs : {tf.config.list_physical_devices('GPU')}")

# 데이터셋 생성
dataset = EyesDataset(x_train, y_train)
train_dataset = dataset.load_data()
train_dataset = train_dataset.shuffle(buffer_size=1000).batch(32)

# 모델 정의
model = Net()

# 컴파일: 옵티마이저와 손실 함수 설정
model.compile(optimizer='adam', loss='binary_crossentropy', metrics=['accuracy'])

# 체크포인트 설정: 가중치를 저장할 경로 지정
checkpoint_path = "weights/classifier_weights.h5"

# 체크포인트가 존재한다면 가중치 로드
if os.path.exists(checkpoint_path):
    # # 모델 변수 초기화를 위한 더미 데이터 입력
    # _ = model(np.random.random((1, 26, 34, 1)).astype(np.float32))
    model.load_weights(checkpoint_path)

checkpoint_callback = tf.keras.callbacks.ModelCheckpoint(filepath=checkpoint_path,
                                                         save_weights_only=False,
                                                         verbose=1)

# 훈련 실행
model.fit(train_dataset, epochs=20, callbacks=[checkpoint_callback])

# 로그 레벨 복구
tf.compat.v1.logging.set_verbosity(tf.compat.v1.logging.DEBUG)
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '0'