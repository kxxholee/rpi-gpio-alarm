# data_loader.py
import tensorflow as tf
import numpy as np

class EyesDataset:
    def __init__(self, x_file:np.ndarray, y_file:np.ndarray):
        self.x_files = x_file
        self.y_files = y_file

    def load_data(self):
        x_data = tf.convert_to_tensor(self.x_files, dtype=tf.float32)
        y_data = tf.convert_to_tensor(self.y_files, dtype=tf.float32)
        return tf.data.Dataset.from_tensor_slices((x_data, y_data))


if __name__ == "__main__":
    x_train = np.load('./dataset/x_train.npy').astype(np.float32)
    y_train = np.load('./dataset/y_train.npy').astype(np.float32)

    dataset = EyesDataset(x_train, y_train)
    train_dataset = dataset.load_data()
    train_dataset = train_dataset.shuffle(buffer_size=1000).batch(32)
