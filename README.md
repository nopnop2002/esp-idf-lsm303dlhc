# esp-idf-lsm303dlhc
A demo showing the pose of the lsm303dlhc in 3D using esp-idf. 

LSM303DLHC is a 6DoF IMU.   
Usually 6Dof IMU has accelerometer and gyroscope, but this IMU has accelerometer and magnetometer.   

Using the data we get from accelerometer and magnetometer we can estimate the Euler angles.
Euler angles are roll, pitch and yaw.   
It's very intuitive and easy to understand.   
However, since BMI160 is a 6DoF IMU, YAW estimation is not possible.   
![a-Pitch-yaw-and-roll-angles-of-an-aircraft-with-body-orientation-O-u-v-original](https://user-images.githubusercontent.com/6020549/226072914-a7f923fc-eb6e-4d19-b2ff-8c9f2749ee6f.jpg)

First, calibrate the compass and find the shift value for each axis.   
Then use the accelerometer and magnetometer to find the Euler angle.   

I used [this](https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/LSM303DLHC) library.   


# Software requiment   
ESP-IDF V4.4/V5.0.   
ESP-IDF V5 is required when using ESP32-C2.   


# Hardware requirements
LSM303DLHC 3D accelerometer and 3D magnetometer module.

# Wireing
|LSM303DLHC||ESP32|ESP32-S2/S3|ESP32-C2/C3||
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
idf.py set-target {esp32/esp32s2/esp32s3/esp32c2/esp32c3}
idf.py menuconfig
idf.py flash
```

### Configuration   
Set the compass offset to 0.

![config-top](https://user-images.githubusercontent.com/6020549/226536118-abca60ec-3468-4bc0-83bb-2b95baf53bf1.jpg)
![config app](https://user-images.githubusercontent.com/6020549/226536122-b641cadf-37b7-44a7-b941-e737b08ef1f1.jpg)


### Execute calibration   
ESP32 acts as a web server.   
I used [this](https://github.com/Molorius/esp32-websocket) component.   
This component can communicate directly with the browser.   
It's a great job.   
Enter the following in the address bar of your web browser.   
```
http:://{IP of ESP32}/
or
http://esp32.local/
```

Move the IMU and plot the X,Y,Z values.   
X, Y, Z offset values are displayed.   

![calibrate-1](https://user-images.githubusercontent.com/6020549/226536404-9c68eaad-ae37-41bc-ac6d-b4a9ada07335.jpg)
![calibrate-2](https://user-images.githubusercontent.com/6020549/226536407-02a3bf80-d1a4-4558-bb5a-e725ecd94ae2.jpg)

Set the calibration value and run again, the position of the circle will change.   

![calibrate-3](https://user-images.githubusercontent.com/6020549/226536920-109b0265-ac45-420f-91d7-ebdc4885e71b.jpg)
![calibrate-4](https://user-images.githubusercontent.com/6020549/226536925-a7814fca-da0f-4ac5-a91c-43b2a8a873f1.jpg)


# Get Euler angles from IMU
```
git clone https://github.com/nopnop2002/esp-idf-lsm303dlhc
cd esp-idf-lsm303dlhc/euler
idf.py set-target {esp32/esp32s2/esp32s3/esp32c2/esp32c3}
idf.py menuconfig
idf.py flash
```

### Configuration   
Set the compass offset to the value obtained during calibration.

![config-top](https://user-images.githubusercontent.com/6020549/226536118-abca60ec-3468-4bc0-83bb-2b95baf53bf1.jpg)
![config app](https://user-images.githubusercontent.com/6020549/226536122-b641cadf-37b7-44a7-b941-e737b08ef1f1.jpg)

### View Euler angles with built-in web server   
ESP32 acts as a web server.   
I used [this](https://github.com/Molorius/esp32-websocket) component.   
This component can communicate directly with the browser.   
It's a great job.   
Enter the following in the address bar of your web browser.   
```
http:://{IP of ESP32}/
or
http://esp32.local/
```

![euler](https://user-images.githubusercontent.com/6020549/226537059-0fb1d339-9d12-4732-91af-1cd7df5ea003.jpg)


WEB pages are stored in the html folder.   
I used [this](https://canvas-gauges.com/) for gauge display.   
I used [this](https://threejs.org/) for 3D display.   
You can change it as you like.   

### View Euler angles using PyTeapot   
You can view Euler angles using [this](https://github.com/thecountoftuscany/PyTeapot-Quaternion-Euler-cube-rotation) tool.   
It works as a UDP display server.   
This is a great application.   

```
+-------------+     +-------------+     +-------------+
|     IMU     | i2c |    ESP32    | UDP | pyteapot.py |
|             |---->|             |---->|             |
|             |     |             |     |             |
+-------------+     +-------------+     +-------------+
```

```
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


