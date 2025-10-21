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

lower_yellow = np.array([15, 100, 100])
upper_yellow = np.array([45, 255, 255])

start = time()
while True:
    frame = picam2.capture_array()
    height, width, _ = frame.shape
    #print(f"Высота: {height}, Ширина: {width}")
    #frame = np.zeros((height, width, 3), np.uint8)
    # create a rectangle
  #  color = colors[curcolor]
  #  curcolor += 1
  #  curcolor %= len(colors)
  #  for y in range(0, int(frame.shape[0] / 2)):
  #      for x in range(0, int(frame.shape[1] / 2)):
  #          frame[y][x] = color
    height_center = height // 2
    width_center = width // 2    
    cv2.rectangle(frame, (width_center - 5, height_center - 5),  (width_center + 5, height_center + 5), (0, 255, 0), 2)
    


    current_time = time()

    if current_time - last_processed_time >= processing_interval:      
        # Создаем массив 1x1 с цветом центрального пикселя и конвертируем его в HSV
        
        last_processed_time = current_time
        center_pixel_bgr = frame[height_center, width_center]
        center_pixel_rgb = center_pixel_bgr[::-1]
        print(f"RGB-значение центрального пикселя: {center_pixel_rgb}")
        center_pixel_hsv = cv2.cvtColor(np.uint8([[center_pixel_bgr]]), cv2.COLOR_BGR2HSV)
        h, s, v = center_pixel_hsv[0, 0]

        if (h >= lower_yellow[0] and h <= upper_yellow[0] and
            s >= lower_yellow[1] and s <= upper_yellow[1] and
            v >= lower_yellow[2] and v <= upper_yellow[2]):
                print(f"Найден желтый цвет!")
            
        

    https://docs.google.com/document/d/1KsOgbiIUKxINDJic4RRzG61ljN5aniIKy6TPMQUbFwg/edit?tab=t.0

    #frame[100] = frame[100]+300
    #print(frame[100])
    out.write(frame)
#   print("%s frame written to the server" % datetime.now())

    now = time()
    diff = (1 / fps) - now - start
    if diff > 0:
        sleep(diff)
    start = now
