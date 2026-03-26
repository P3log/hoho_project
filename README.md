# DHT\_tracer README

Author: P3log</br>
Date: 25/03/2026

## 1 - Description
The *DHT22\_tracer* was designed to trace measures realised by a DHT sensor (also named AM2302) on a Raspberry Pi device that would work continuously. It was implemented in C language in order to keep it light, fast and low consumer. This project contains :
- An **assembler** script in charge to create an archive to be uploaded on the Raspberry device
- A handmade-library compatible with the DHT sensor **and** the Raspberry Pi family. It requires the pigpio library.
- Different packages to be contained in the previously mentioned archive
- A **transmitter** script to move in an easy way data toward the Raspberry Pi.
- An installer to ease the configuration procedure

The scripts are implemented in bash, which mean a full compatibility with linux operating systems.

## 2 - How it works
A DHT22 sensor is connected to a Raspberry Pi device and takes measures every t time. The recorded parameters are humidity (%) and temperature (°C). They are saved in a csv file. A user can connect regularly (or not) to the raspberry pi device and download the recorded data to analyse it. A logger can also trace errors in a log file.

## 3 - Installation guide
1. Clone the project or download it whether to your computer or straight to your Raspberry Pi device
```bash
    git clone https://github.com/P3log/DHT22_tracer.git
```
2. Enter the project
```bash
    cd DHT22_tracer
```
2. Set *assembler*, *installer* and *transmitter* as executable files :
```bash
    chmod u+x assembler
    chmod u+x installer
    chmod u+x transmitter
```
3. Run the assembler to generate an archive
```bash
    ./assembler
```
An archive is generated, named *tracer.zip*

4. Right before the next step you have to create a directory on the Raspberry Pi :
```bash
    # In the Raspberry Pi
    cd
    mkdir th_sensor
    cd th_sensor

```
If you do not use the *transmitter* script (or if you modify it) you can name this directory anything you would like.

If you have installed the project directly on the raspberry device, go straight to step 7

5. On your computer, run the transmitter. It will transmit the previously generated archive to your Raspberry Pi device through an SSH connection. Prior to this your computer **must** be connected to the Raspberry.
```bash
    ./transmitter RASPBERRY_USERNAME RASPBERRY_IP
    # Enter the raspberry password when it is asked
```

6. Connect to the Raspberry Pi device
```bash
    ssh RASPBERRY_USERNAME@RASPBERRY_IP
    # Enter the raspberry password when it is asked
```

7. On the Raspberry Pi, install the pigpio library :
```bash
    sudo apt update
    sudo apt install pigpio
```

8. Decompress the archive
```bash
    unzip tracer.zip
```

9. Run the installer
```bash
    ./installer
```
The installer will build the project appropriately

## 4 - Run the program
the executables are in `/bin`. You need the root privileges to access to sensor. Run :
```bash
    sudo ./manager DELAY_IN_MINUTES_BETWEEN_TWO_MEASURES
```
The program runs until it is interrupted manually by the user (or until it crashes).


## 5 - Interesting data
### 5.1 - Database
The file **measures.csv** is used as the database of this project. It is stored in `./db/measures.csv`.
It stores in this order : 
```
Date;Time;Humidity;Temperature
```
One line corresponds to one measure.

### 5.1 - Log file
The log file stores data related to errors and is stored in `./logs/journal.log`

