# esp-idf-lsm303dlhc
A demo showing the pose of the lsm303dlhc 6DoF IMU sensor in 3D using esp-idf. 

Usually 6Dof IMU has accelerometer and gyroscope, but this IMU has accelerometer and magnetometer.   

Using the data we get from accelerometer and magnetometer we can estimate the Euler angles.   
Euler angles are roll, pitch and yaw.   
It's very intuitive and easy to understand.   
![a-Pitch-yaw-and-roll-angles-of-an-aircraft-with-body-orientation-O-u-v-original](https://user-images.githubusercontent.com/6020549/226072914-a7f923fc-eb6e-4d19-b2ff-8c9f2749ee6f.jpg)   
You can view like this.   
![Image](https://github.com/user-attachments/assets/cb9f1408-b372-4e55-84f6-1191f068cd1a)

# Installation overview   
First, calibrate the compass and find the offset value for each axis.   
As you can see, the X, Y and Z axes are quite off-center.   
![calibrate-0](https://user-images.githubusercontent.com/6020549/227759768-63737d65-e080-465e-b7dc-e5eb4dc230e7.jpg)

Then use the accelerometer and magnetometer to find the Euler angle.   
![lsm303dlhc-euler](https://user-images.githubusercontent.com/6020549/232389292-acb291b7-b92d-44e7-83ab-ca3e8048960c.JPG)

# Software requiment   
ESP-IDF V5.0 or later.   
ESP-IDF V4.4 release branch reached EOL in July 2024.   
ESP-IDF V5.1 is required when using ESP32-C6.   

# Hardware requirements
LSM303DLHC 3D accelerometer and 3D magnetometer module.

# Wireing
|LSM303DLHC||ESP32|ESP32-S2/S3|ESP32-C2/C3/C6||
|:-:|:-:|:-:|:-:|:-:|:-:|
|VIN|--|N/C|N/C|N/C||
|3V3|--|3.3V|3.3V|3.3V||
|GND|--|GND|GND|GND||
|SCL|--|GPIO22|GPIO12|GPIO5|(*1)|
|SDA|--|GPIO21|GPIO11|GPIO4|(*1)|

(*1)You can change it to any pin using menuconfig.   


# Caribrate compass
```
git clone https://github.com/nopnop2002/esp-idf-lsm303dlhc
cd esp-idf-lsm303dlhc/calibrate
idf.py set-target {esp32/esp32s2/esp32s3/esp32c2/esp32c3/esp32c6}
idf.py menuconfig
idf.py flash
```

### Configuration   
To find the offset value, set the compass offset to 0.   
![config-top](https://user-images.githubusercontent.com/6020549/226536118-abca60ec-3468-4bc0-83bb-2b95baf53bf1.jpg)
![config app](https://user-images.githubusercontent.com/6020549/227060584-98e0cac4-1adf-404b-9dbc-0309b42a8aac.jpg)

### Execute calibration   
ESP32 acts as a web server.   
I used [this](https://github.com/Molorius/esp32-websocket) component.   
This component can communicate directly with the browser.   
Enter the following in the address bar of your web browser.   
```
http:://{IP of ESP32}/
or
http://esp32.local/
```

As you move the IMU it plots the X, Y and Z values.   
X, Y, Z offset are displayed.   

![calibrate-1](https://user-images.githubusercontent.com/6020549/227657039-2b7e7632-a642-4836-b448-dfce263aa65f.jpg)
![calibrate-2](https://user-images.githubusercontent.com/6020549/227657042-fad1d5da-e199-4aa3-b923-c09037218c35.jpg)

### Execute calibration again   
If you set the offset you got from the calibration and run it again, the circle position will change.   

![calibrate-11](https://user-images.githubusercontent.com/6020549/227657110-4dcddf31-bb6e-4524-b7d4-8272a443fb41.jpg)
![calibrate-12](https://user-images.githubusercontent.com/6020549/227657112-e0b9d405-0a5e-4437-9255-5a2ca7bd9ed5.jpg)

# Get Euler angles from IMU
```
git clone https://github.com/nopnop2002/esp-idf-lsm303dlhc
cd esp-idf-lsm303dlhc/euler
idf.py set-target {esp32/esp32s2/esp32s3/esp32c2/esp32c3/esp32c6}
idf.py menuconfig
idf.py flash
```

### Configuration   
Sets the compass offset obtained by calibration.   

![config-top](https://user-images.githubusercontent.com/6020549/226536118-abca60ec-3468-4bc0-83bb-2b95baf53bf1.jpg)
![config app](https://user-images.githubusercontent.com/6020549/227060584-98e0cac4-1adf-404b-9dbc-0309b42a8aac.jpg)

### View Euler angles with built-in web server   
ESP32 acts as a web server.   
I used [this](https://github.com/Molorius/esp32-websocket) component.   
This component can communicate directly with the browser.   
Enter the following in the address bar of your web browser.   
```
http:://{IP of ESP32}/
or
http://esp32.local/
```

![lsm303dlhc-euler](https://user-images.githubusercontent.com/6020549/232389292-acb291b7-b92d-44e7-83ab-ca3e8048960c.JPG)


WEB pages are stored in the html folder.   
I used [this](https://threejs.org/) for 3D display.   
I used [this](https://canvas-gauges.com/) for gauge display.   
Configuration Options for the gauge display is [here](https://canvas-gauges.com/documentation/user-guide/configuration).   
You can change the design and color according to your preference like this.   
![Image](https://github.com/user-attachments/assets/d0e1ca46-0d46-41ed-bbbc-9f26af28900d)

### View Euler angles using PyTeapot   
You can view Euler angles using [this](https://github.com/thecountoftuscany/PyTeapot-Quaternion-Euler-cube-rotation) tool.   
It works as a UDP display server.   
This is a great application.   

```
+-------------+          +-------------+          +-------------+
|             |          |             |          |             |
|     IMU     |--(i2c)-->|    ESP32    |--(UDP)-->| pyteapot.py |
|             |          |             |          |             |
+-------------+          +-------------+          +-------------+
```


### Installation for Linux
```
$ python3 --version
Python 3.11.2
$ sudo apt install python3-pip python3-setuptools
$ python3 -m pip install -U pip
$ python3 -m pip install pygame
$ python3 -m pip install PyOpenGL PyOpenGL_accelerate
$ git clone https://github.com/thecountoftuscany/PyTeapot-Quaternion-Euler-cube-rotation
$ cd PyTeapot-Quaternion-Euler-cube-rotation
$ python3 pyteapot.py
```
The posture of your sensor is displayed.   
![lsm303dlhc_2023-03-21_15-00-00](https://user-images.githubusercontent.com/6020549/226537181-6af8468c-f990-4ccb-825a-dc448acf4534.png)

### Installation for Windows   
Install Git for Windows from [here](https://gitforwindows.org/).   
Install Python Releases for Windows from [here](https://www.python.org/downloads/windows/).   
Open Git Bash and run:   
```
$ python --version
Python 3.11.9
$ python -m pip install -U pip
$ python -m pip install pygame
$ python -m pip install PyOpenGL PyOpenGL_accelerate
$ git clone https://github.com/thecountoftuscany/PyTeapot-Quaternion-Euler-cube-rotation
$ cd PyTeapot-Quaternion-Euler-cube-rotation
$ python pyteapot.py
```
![PyTeapot-Windows](https://github.com/user-attachments/assets/2b0a1a70-40cb-47e5-8f51-eb4fe3adb1ab)


# View Euler angles using panda3d library   
You can view Euler angles using [this](https://www.panda3d.org/) library.   
It works as a UDP display server.   

```
+-------------+          +-------------+          +-------------+
|             |          |             |          |             |
|     IMU     |--(ic2)-->|    ESP32    |--(UDP)-->|  panda.py   |
|             |          |             |          |             |
+-------------+          +-------------+          +-------------+
```

### Installation for Linux
```
$ python3 --version
Python 3.11.2
$ sudo apt install python3-pip python3-setuptools
$ python3 -m pip install -U pip
$ python3 -m pip install panda3d
$ python3 -m pip install numpy
$ git clone https://github.com/nopnop2002/esp-idf-mpu6050-dmp
$ cd esp-idf-mpu6050-dmp/panda3d
$ python3 panda.py --help
usage: panda.py [-h] [--model {jet,biplain,707,fa18}]

options:
  -h, --help            show this help message and exit
  --model {jet,biplain,707,fa18}
```
![Image](https://github.com/user-attachments/assets/6d81eec0-5b80-4e5f-ae97-689742253f9a)   

### Installation for Windows
Install Git for Windows from [here](https://gitforwindows.org/).   
Install Python Releases for Windows from [here](https://www.python.org/downloads/windows/).   
Open Git Bash and run:   
```
$ python --version
Python 3.11.9
$ python -m pip install -U pip
$ python -m pip install panda3d
$ python -m pip install numpy
$ git clone https://github.com/nopnop2002/esp-idf-mpu6050-dmp
$ cd esp-idf-mpu6050-dmp/panda3d
$ python panda.py --help
usage: panda.py [-h] [--model {jet,biplain,707,fa18}]

options:
  -h, --help            show this help message and exit
  --model {jet,biplain,707,fa18}
```
![Image](https://github.com/user-attachments/assets/0ec982c4-3353-4cb8-9c39-ecd785ca9729)

### How to use   
See [here](https://github.com/nopnop2002/esp-idf-mpu6050-dmp/blob/main/panda3d/README.md)   



