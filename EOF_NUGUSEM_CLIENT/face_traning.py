import cv2 
import numpy as np 
from os import walk, getcwd
from os.path import join, basename
import os
os.chdir(os.path.dirname(__file__)) # vscode에서 현재 path 잘못 잡는 문제해결용


def train_model():
    face_classifier = cv2.CascadeClassifier(cv2.data.haarcascades + "haarcascade_frontalface_default.xml")
    person_id = -1
    prev_name = ""
    label, data = [], []
    
    directory_path = join(getcwd(), "faces")

    for root, dir, files in walk(directory_path):
        print(root)
        print(dir)
        for file in files:
            if file.endswith("jpeg") or file.endswith("jpg") or file.endswith("png") :
                file_path = join(root, file)
                current_name = basename(root)

                if prev_name != current_name:
                    person_id += 1
                    prev_name = current_name

                src_gray = cv2.cvtColor(cv2.imread(file_path), cv2.COLOR_BGR2GRAY)
                face_area_info = face_classifier.detectMultiScale(src_gray, 1.3, 5)

                for (x, y, width, height) in face_area_info:
                    roi = src_gray[x:x+width, y:y+height]
                    data.append(roi)
                    label.append(person_id)

    model = cv2.face.LBPHFaceRecognizer_create() 
    model.train(data, np.array(label))
    model.save(join(getcwd(), "model/face_recognizer_model.yml"))