# model.py
import tensorflow as tf
from tensorflow import keras
from keras import layers, models

"""
*  TODO
1. Net 에서 tflite 변환시 호환되지 않는 함수들 수정
2. tflite변환 및 RaspberryPi에서 테스트
3. OpenCV와의 호환 테스트 -> 티스토리 링크에서 코드 참고
    * numpy 메소드를 어떻게 C++에서 사용할 것인가?
    * numpy를 사용하지 않고 구현이 가능한가?
4. Drvice Driver과의 호환 테스트

"""

def Net():
    return models.Sequential([
        layers.Conv2D(32, kernel_size=3, strides=1, padding='same'),
        layers.Activation('relu'),
        layers.MaxPooling2D((2, 2)),
        layers.Conv2D(64, kernel_size=3, strides=1, padding='same'),
        layers.Activation('relu'),
        layers.MaxPooling2D((2, 2)),
        layers.Conv2D(128, kernel_size=3, strides=1, padding='same'),
        layers.Activation('relu'),
        layers.MaxPooling2D((2, 2)),

        layers.Flatten(),

        layers.Dense(512, activation='relu'),
        layers.Dense(1, activation='sigmoid')
    ])

# class Net(models.Model):
#     def __init__(self):
#         super(Net, self).__init__()
#         self.conv1 = layers.Conv2D(32, kernel_size=3, strides=1, padding='same', activation='relu')
#         self.conv2 = layers.Conv2D(64, kernel_size=3, strides=1, padding='same', activation='relu')
#         self.conv3 = layers.Conv2D(128, kernel_size=3, strides=1, padding='same', activation='relu')
#         self.flatten = layers.Flatten()
#         self.fc1 = layers.Dense(512, activation='relu')
#         self.fc2 = layers.Dense(1, activation='sigmoid')

#     def call(self, x):
#         x = layers.MaxPooling2D((2, 2))(self.conv1(x))
#         x = layers.MaxPooling2D((2, 2))(self.conv2(x))
#         x = layers.MaxPooling2D((2, 2))(self.conv3(x))
#         x = self.flatten(x)
#         x = self.fc1(x)
#         x = self.fc2(x)
#         return x

# 모델 요약을 보려면:
# model = Net()
# model.build(input_shape=(None, 26, 34, 1))
# model.summary()
