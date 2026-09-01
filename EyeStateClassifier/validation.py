import os
# 0 = all messages are logged (default behavior)
# 1 = INFO messages are not printed
# 2 = INFO and WARNING messages are not printed
# 3 = INFO, WARNING, and ERROR messages are not printed
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3' # 하고싶으면 하는거임
import numpy as np
import tensorflow as tf
from model import Net
from data_loader import EyesDataset
from tqdm import tqdm

# 정확도 계산 함수
def accuracy(y_pred, y_true):
    y_pred_tag = tf.round(y_pred)
    correct_results_sum = tf.reduce_sum(tf.cast(y_pred_tag == y_true, tf.float32))
    acc = correct_results_sum / tf.cast(tf.size(y_true), tf.float32)
    acc = tf.round(acc * 100)
    return acc

# 데이터 로드
x_test = np.load('./dataset/x_val.npy').astype(np.float32)
y_test = np.load('./dataset/y_val.npy').astype(np.float32)

# 데이터셋 생성
dataset = EyesDataset(x_test, y_test)
test_dataset = dataset.load_data()
test_dataset = test_dataset.batch(1)

# 모델 로드
model = Net()
model.compile(optimizer='adam', loss='binary_crossentropy', metrics=['accuracy'])
# _ = model(np.random.random((1, 26, 34, 1)).astype(np.float32))
model.load_weights('weights/classifier_weights.h5')

# 검증
total_acc = 0.0
count = 0

for data, labels in tqdm(test_dataset):
    preds = model(data, training=False)
    total_acc += accuracy(preds, labels)
    count += 1

avg_acc = total_acc / count
print(f'Average accuracy: {avg_acc.numpy():.2f}%')
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '0'