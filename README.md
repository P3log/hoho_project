# DHT\_tracer README

Author: P3log</br>
Date: 25/03/2026



## 1 - Description
The *DHT22\_tracer* was designed to trace measures realised by a DHT sensor (also named AM2302) on a Raspberry Pi device that would work continuously. It was implemented in C/C++ language in order to keep it light, fast and low consumer. </br>
This project contains :
- A server application to manage the DHT sensor on a Raspberry Pi device
- A client application to manage the DHT sensor with a wireless approach (http protocol).
- A C-code-handmade-library compatible with the DHT sensor **and** the Raspberry Pi family. It requires the pigpio library.
- An assembler script that manages to gather in one file all the required files to install the server on the Raspberry Pi
- An installer to ease the configuration procedure
- A project designed to remain as free as possible from dependances !

The scripts are implemented in bash, which mean a full compatibility with linux operating systems.

## 2 - How it works
A DHT22 sensor is connected to a Raspberry Pi device and takes measures every t time. The recorded parameters are humidity (%) and temperature (°C). They are saved in a csv file. A user can connect regularly (or not) to the raspberry pi device and download the recorded data to analyse it. A logger can also trace errors in a log file.

## 3 - Installation guide
1. Clone the project or download it whether to your computer or straight to your Raspberry Pi device
```bash
    git clone https://github.com/P3log/DHT22_tracer.git
```

### 3.1 - Server
1. Enter the project
```bash
    cd DHT22_tracer/server-side
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

10. Once the process is done, a specific message is displayed in the terminal to tell so. Run the server by going in the `/bin` directory :
```bash
sudo ./server
```
The device is ready !

### 3.2 - Client
The client runs through a navigator. It was designed to be as light as possible from the standard base : html, CSS and javascript.

1. Run the client 
```bash
./client
```

It will open a server through Python and open the application.
You are ready to use the application !


## 4 - Client interface:

### 4.1 - Management of the tracer
Located in the top right side of the screen, a few items are available :
- **Start** : it runs the device considering an **interval** that must be defined in the field attached to this button. The interval, in **minutes**, corresponds to the delay between two measures being taken by the DHT device. When a series of measures is started, all the data are stored on a file named depending the date the first measure was made (i.e. approximatively the moment the start button was clicked on). All the measures are stored on the same file until the series of measures are stopped voluntarily.
- **Stop** : it stops the campaign of measures. Note that this button is partially protected by a warning each time it is used, in order to prevent from a misclick.
- Above these two buttons, an indicator visually informs the user whether the tracer is currently taking measures or not.

### 4.2 - Access data
- **Get files** : this button retrieves the databases already on the Raspberry. No file is created until the first series of measures is launched. A file can be explored during a campaign of measures without interrupting the process.
- **Read file** : if any database is found on the raspberry it will be displayed. Clicking on the *"read"* button will display automatically its content below.
- **Upload CSV** : it is also possible to read a local database saved on your own computer for instance. Once launched the file automatically displays the database's content
- **Logs** : This button displays all the requests that have been addressed to the distant server until the last click on this button.

### 4.3 - Data content
3 elements are extracted from the databases each time :

1. A linegraph that visually displays the evolution of temperature and humidity over the different measures of a campaign of measures.
2. The same results displayed in an array below
3. Basic statistics about the measures : 
    - Number of measures
    - Average temperature (°C)
    - Average humidity (%)

- **Download** : for any measure file read straight from the raspberry, it is possible to download the measures on your own computer.