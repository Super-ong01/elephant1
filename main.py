from imageai.Detection import VideoObjectDetection
import os

execution_path = os.getcwd()

yolo = VideoObjectDetection()
yolo.setModelTypeAsYOLOv3()
yolo.setModelPath(os.path.join(execution_path, "yolov3.pt"))
yolo.loadModel()

# obj = yolo.CustomObjects()

import cv2
import matplotlib.pyplot as plt


camera = cv2.VideoCapture(0)

def forFrame(frame_number, output_array, output_count, detected_frame):
    plt.clf()
    plt.title("Frame : " + str(frame_number))
    plt.imshow(detected_frame, interpolation="none")
    print("FOR FRAME " , frame_number)
    print("Output for each object : ", output_array)
    print("Output count for unique objects : ", output_count)
    print("------------END OF A FRAME --------------")
    plt.pause(0.01)

video_path = yolo.detectObjectsFromVideo(
                camera_input=camera,
                save_detected_video=False,
                return_detected_frame=True,
                frames_per_second=30, 
    per_frame_function=forFrame,
    log_progress=True, minimum_percentage_probability=40)