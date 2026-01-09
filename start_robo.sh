#!/bin/bash

# Переходим в папку с медиа-сервером и запускаем его в фоне
echo "Starting MediaMTX..."
cd ~/RoboCup/Server
# Запускаем ./mediamtx (убедитесь, что файл исполняемый: chmod +x mediamtx)
./mediamtx & 

# Даем пару секунд серверу на инициализацию
sleep 2

# Переходим в папку с Python скриптом и запускаем его
echo "Starting Python Translation..."
cd ~/RoboCup/Server/python
python3 translation.py

# При прерывании скрипта (Ctrl+C) - убить и фоновый процесс медиа-сервера
trap "kill $!" EXIT
