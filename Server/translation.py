from datetime import datetime
from time import sleep, time

import cv2
import numpy as np
from picamera2 import Picamera2
picam2 = Picamera2()
picam2.preview_configuration.main.size = (800,600)
picam2.preview_configuration.main.format = "RGB888"
picam2.preview_configuration.align()
picam2.configure("preview")
picam2.start()

fps = 30
width = 800
height = 600
colors = [
    (0, 0, 255),
    (255, 0, 0),
    (0, 255, 0),
]
out = cv2.VideoWriter('appsrc ! videoconvert' + \
    ' ! video/x-raw,format=I420' + \
    ' ! x264enc speed-preset=ultrafast bitrate=600 key-int-max=' + str(fps * 2) + \
    ' ! video/x-h264,profile=baseline' + \
    ' ! rtspclientsink location=rtsp://localhost:8554/mystream',
    cv2.CAP_GSTREAMER, 0, fps, (width, height), True)
if not out.isOpened():
    raise Exception("can't open video writer")
curcolor = 0

last_processed_time = time()
# Интервал в секундах, например 1 секунда
processing_interval = 1.0 

lower_orange = np.array([5, 150, 150])
upper_orange = np.array([25, 255, 255])

kernel = np.ones((3, 3), dtype=np.uint8)

start = time()
while True:
    frame = picam2.capture_array()
    height, width, _ = frame.shape
    height_center = height // 2
    width_center = width // 2    
    cv2.circle(frame, (width_center, height_center), 5, (0, 255, 0), -1)


    frame_hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    mask = cv2.inRange(frame_hsv, lower_orange, upper_orange)
    
    mask_bgr = cv2.cvtColor(mask, cv2.COLOR_GRAY2BGR)

    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    eroded = cv2.erode(mask, kernel, iterations=3)

    dilated = cv2.dilate(eroded, kernel, iterations=3)
    
    dilated_bgr = cv2.cvtColor(dilated, cv2.COLOR_GRAY2BGR)

    if contours:
        c = max(contours, key=cv2.contourArea)
        M = cv2.moments(c)
        if M["m00"] != 0:
            cx = int(M["m10"] / M["m00"])
            cy = int(M["m01"] / M["m00"])
            cv2.circle(frame, (cx, cy), 5, (0, 255, 0), -1)

    out.write(frame)

    now = time()
    diff = (1 / fps) - now - start
    if diff > 0:
        sleep(diff)
    start = now
