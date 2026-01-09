import serial
import time

ser = serial.Serial('/dev/ttyS0', 9600, timeout=1)
time.sleep(2)

while True:
    try:
        ser.write(b'2\n')
        print("Отправлено!")
        time.sleep(1)
    except Exception as e:
        break
        print(f"Ошибка: {e}")