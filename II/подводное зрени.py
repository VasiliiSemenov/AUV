import tkinter as tk
from tkinter import filedialog
import cv2
import torch
from pathlib import Path

# Функция для выбора файла
def select_file():
    root = tk.Tk()
    root.withdraw()  # Скрытие главного окна
    file_path = filedialog.askopenfilename(title="Выберите видеофайл", filetypes=[("Видео", "*.mp4 *.avi *.mov")])
    return file_path

# Функция для детектирования объектов
def detect_objects(video_path):
    # Загрузка предобученной модели YOLOv5
    model = torch.hub.load('ultralytics/yolov5', 'yolov5s')

    # Открытие видео
    cap = cv2.VideoCapture(video_path)
    while True:
        ret, frame = cap.read()
        if not ret:
            break

        # Детектирование объектов на кадре
        results = model(frame)

        # Отображение результатов
        annotated_frame = results.render()[0]
        cv2.imshow('Object Detection', annotated_frame)

        # Выход при нажатии клавиши "q"
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

# Главный код
if __name__ == "__main__":
    video_file = select_file()
    if video_file:
        print(f"Выбран файл: {video_file}")
        detect_objects(video_file)
    else:
        print("Файл не был выбран.")
