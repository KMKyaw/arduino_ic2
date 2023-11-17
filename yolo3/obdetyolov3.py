import cv2
import numpy as np

# Load YOLO
net_yolo = cv2.dnn.readNet("yolov3.weights", "yolov3.cfg")
classes_yolo = []
with open("coco.names", "r") as f:
    classes_yolo = [line.strip() for line in f.readlines()]

# Generate random colors for each class in YOLO
COLORS_yolo = np.random.uniform(0, 255, size=(len(classes_yolo), 3))

# Load MobileNet SSD
net_mobilenet = cv2.dnn.readNetFromCaffe('deploy.prototxt', 'mobilenet_iter_73000.caffemodel')

# Constants for camera calibration in MobileNet SSD
KNOWN_WIDTH_PERSON = 20  # The known width of a person (in cm)

# Constants for YOLO object detection
KNOWN_CHAIR_HEIGHT = 0.8  # Example: average height of a chair
KNOWN_TABLE_HEIGHT = 0.75  # Example: average height of a table
FOCAL_LENGTH_YOLO = 1000.0  # Focal length for YOLO (hypothetical value, replace with actual focal length)

# Set up camera
cap = cv2.VideoCapture(0)  # Use 0 for default camera, or replace with the camera index you want

while True:
    ret, frame = cap.read()
    height, width, channels = frame.shape

    # YOLO Object Detection
    blob = cv2.dnn.blobFromImage(frame, 0.00392, (416, 416), (0, 0, 0), True, crop=False)
    net_yolo.setInput(blob)
    outs = net_yolo.forward(net_yolo.getUnconnectedOutLayersNames())

    class_ids_yolo = []
    confidences_yolo = []
    boxes_yolo = []

    for out in outs:
        for detection in out:
            scores = detection[5:]
            class_id_yolo = np.argmax(scores)
            confidence_yolo = scores[class_id_yolo]
            if confidence_yolo > 0.5:
                center_x = int(detection[0] * width)
                center_y = int(detection[1] * height)
                w = int(detection[2] * width)
                h = int(detection[3] * height)
                x = int(center_x - w / 2)
                y = int(center_y - h / 2)
                class_ids_yolo.append(class_id_yolo)
                confidences_yolo.append(float(confidence_yolo))
                boxes_yolo.append([x, y, w, h])

    indices_yolo = cv2.dnn.NMSBoxes(boxes_yolo, confidences_yolo, 0.5, 0.4)

    for i in range(len(boxes_yolo)):
        if i in indices_yolo:
            x, y, w, h = boxes_yolo[i]
            class_id_yolo = class_ids_yolo[i]
            confidence_yolo = confidences_yolo[i]

            # Filter classes to detect only "table" and "chair"
            if class_id_yolo in [56, 61] and confidence_yolo > 0.7:
                # Calculate and display distance for tables and chairs
                if class_id_yolo == 56:  # class ID for "table"
                    object_height_yolo = KNOWN_TABLE_HEIGHT
                elif class_id_yolo == 61:  # class ID for "chair"
                    object_height_yolo = KNOWN_CHAIR_HEIGHT
                else:
                    continue

                distance_yolo = (object_height_yolo * FOCAL_LENGTH_YOLO) / h
                annotation_yolo = f"{classes_yolo[class_id_yolo]} | Confidence: {confidence_yolo:.2f} | Distance: {distance_yolo:.2f} cm"
                cv2.rectangle(frame, (x, y), (x + w, y + h), COLORS_yolo[class_id_yolo], 2)
                cv2.putText(frame, annotation_yolo, (x, y - 15), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)

    # MobileNet SSD Distance Measurement
    blob_mobilenet = cv2.dnn.blobFromImage(frame, 0.007843, (300, 300), 127.5)
    net_mobilenet.setInput(blob_mobilenet)
    detections_mobilenet = net_mobilenet.forward()

    for i in range(detections_mobilenet.shape[2]):
        confidence_mobilenet = detections_mobilenet[0, 0, i, 2]

        if confidence_mobilenet > 0.2:
            class_id_mobilenet = int(detections_mobilenet[0, 0, i, 1])

            box_mobilenet = detections_mobilenet[0, 0, i, 3:7] * np.array([width, height, width, height])
            (startX_mobilenet, startY_mobilenet, endX_mobilenet, endY_mobilenet) = box_mobilenet.astype("int")

            obj_width_mobilenet = endX_mobilenet - startX_mobilenet

            if class_id_mobilenet == 15:  # Person
                known_width_mobilenet = KNOWN_WIDTH_PERSON
                obj_type_mobilenet = "Person"
            else:
                continue

            distance_mobilenet = (known_width_mobilenet * width) / (2 * obj_width_mobilenet * np.tan(0.5 * (60 * np.pi / 180)))

            cv2.rectangle(frame, (startX_mobilenet, startY_mobilenet), (endX_mobilenet, endY_mobilenet), (0, 255, 0), 2)
            text_mobilenet = f"{obj_type_mobilenet}, Distance: {distance_mobilenet:.2f} cm"
            cv2.putText(frame, text_mobilenet, (startX_mobilenet, startY_mobilenet - 15),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

    # Display the resulting frame
    cv2.imshow("Object Detection", frame)

    # Break the loop if 'q' key is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the capture and close the window
cap.release()
cv2.destroyAllWindows()
