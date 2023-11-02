import cv2
import numpy as np
import pygatt
import time
# Load YOLO model configuration and weights
model_config = r'yolov3-tiny.cfg'  # Path to YOLOv3 model configuration file
model_weights = r'yolov3-tiny.weights'  # Path to YOLOv3 model weights file

# Load YOLO class labels
with open(r'coco.names', 'r') as f:
    classes = f.read().strip().split('\n')

# Load the pre-trained YOLO model
net = cv2.dnn.readNet(model_weights, model_config)

# Set the preferred backend and target for OpenCV's DNN module
net.setPreferableBackend(cv2.dnn.DNN_BACKEND_OPENCV)
net.setPreferableTarget(cv2.dnn.DNN_TARGET_CPU)  # You can use DNN_TARGET_CUDA for GPU acceleration if available

# Initialize Bluetooth
esp32_mac_address = "08:3a:f2:a9:f9:9a"  # Replace with your ESP32's MAC address
adapter = pygatt.GATTToolBackend()
adapter.start()
device = adapter.connect(esp32_mac_address)

# Open the webcam
cap = cv2.VideoCapture(0)
start_time = time.time()
while True:
    # Read a frame from the webcam
    ret, frame = cap.read()

    if not ret:
        print("Error: Could not read frame.")
        break

    height, width = frame.shape[:2]

    # Create a blob from the frame and perform a forward pass
    blob = cv2.dnn.blobFromImage(frame, 1/255.0, (416, 416), swapRB=True, crop=False)
    net.setInput(blob)
    output_layers = net.getUnconnectedOutLayersNames()
    outputs = net.forward(output_layers)

    # Process detection results for elephants
    conf_threshold = 0.5
    nms_threshold = 0.4

    class_ids = []
    confidences = []
    boxes = []

    elephant_detected = False  # Flag to check if a elephant is detected

    for output in outputs:
        for detection in output:
            scores = detection[5:]
            class_id = np.argmax(scores)
            confidence = scores[class_id]

            if confidence > conf_threshold and classes[class_id] == 'elephant':
                center_x = int(detection[0] * width)
                center_y = int(detection[1] * height)
                w = int(detection[2] * width)
                h = int(detection[3] * height)

                x = int(center_x - w / 2)
                y = int(center_y - h / 2)

                class_ids.append(class_id)
                confidences.append(float(confidence))
                boxes.append([x, y, w, h])

                # Send "detected" to the ESP32 when a elephant is detected
                print("detected")
                device.char_write("beb5483e-36e1-4688-b7f5-ea07361b26a8", b"detected")

                elephant_detected = True  # Set the flag to True when a elephant is detected
                
    # Print "not detected" if no elephant is detected for this frame
    #if not elephant_detected:
        #print("not detected")
        #device.char_write("beb5483e-36e1-4688-b7f5-ea07361b26a8", b"not detected")
    # Send "OK" to the ESP32 every 3 minutes
    current_time = time.time()
    if current_time - start_time >= 20:  # 180 seconds = 3 minutes
        print("Sending 'ON$OK' to ESP32")
        device.char_write("beb5483e-36e1-4688-b7f5-ea07361b26a8", b"OK")
        start_time = current_time  # Reset the timer
    # Apply Non-Maximum Suppression to remove overlapping detections
    indices = cv2.dnn.NMSBoxes(boxes, confidences, conf_threshold, nms_threshold)

    # Draw bounding boxes and labels on the frame for elephants
    for i in indices:
        box = boxes[i]
        x, y, w, h = box
        label = f'elephant: {confidences[i]:.2f}'  # Construct the label
        color = (0, 255, 0)  # BGR color (green)
        cv2.rectangle(frame, (x, y), (x + w, y + h), color, 2)
        cv2.putText(frame, label, (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, color, 2)

    # Display the frame with detections
    cv2.imshow('elephant Detection', frame)

    # Break the loop if the 'q' key is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the webcam, close windows, and stop Bluetooth communication
cap.release()
cv2.destroyAllWindows()
device.disconnect()
adapter.stop()
