import serial
import numpy as np
import tensorflow as tf

# Загрузка обученной модели (например, TensorFlow Lite)
interpreter = tf.lite.Interpreter(model_path="energy_optimization.tflite")
interpreter.allocate_tensors()

# Функция для обработки данных
def predict_corrections(input_data):
    input_data = np.array(input_data, dtype=np.float32).reshape(1, -1)
    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()

    interpreter.set_tensor(input_details[0]['index'], input_data)
    interpreter.invoke()
    result = interpreter.get_tensor(output_details[0]['index'])
    return result[0]

# Связь с Arduino
arduino = serial.Serial('/dev/ttyAMA0', 9600)

while True:
    # Получение данных от Arduino
    raw_data = arduino.readline().decode().strip()
    sensor_data = list(map(float, raw_data.split(',')))  # Например: [скорость, глубина, температура]

    # Прогнозирование корректировок
    corrections = predict_corrections(sensor_data)

    # Отправка корректировок на Arduino
    corrections_str = ','.join(map(str, corrections))
    arduino.write((corrections_str + '\n').encode())
