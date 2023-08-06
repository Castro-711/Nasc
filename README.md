# Nasc
A server to handle device information

## Overview
Each directory contains different versions of the project. The final piece was *Nasc-12-refactored.ino* - this was used at the festival

## Improving development process
To make the development process faster I am adding a wifi extender & a raspberry PiTop out to the equipment in the shed. The main benefit of this will be to remotely develop, compile & install new sequences via the PiTop, hopefully increasing the development turnaround by removing the need to hardwire directly from the laptop to the arduino. 

### Testing out the Wifi Extender
| Device/Location 	| Kitchen Router (base line) 	| Shed Router                	| Shed Wifi Extender 	|   	|
|-----------------	|----------------------------	|----------------------------	|--------------------	|---	|
| Mobile          	| 533.41mb Down              	| 167.61mb Down              	|                    	|   	|
| Laptop          	| 509.78mb Down / 50.84mb Up 	| 222.24mb Down / 38.56mb Up 	|                    	|   	|
|                 	|                            	|                            	|                    	|   	|

## Remote Development
On the 4th of August 2023, I successfully deployed code using **platformio** via my **pi-top**. I needed to use a custom command in the platform.ini file to remove the `false` which I think is **Windows** specific 🤷‍♂️  
Anyways the main thing is I can now develop my patterns remotely. I confirmed by changing the `START_SIGNAL` colour from `aqua` to `yellow` - Dylan's favourite colour/word at the time.    

## Backed up images  
I backed up the pi-top images, with the current working config in case the worst happens. I have saved them to proton drive.  
```bash
nasc-rpi-boot
nasc-rpi-recovery
```

## How to upload new code to the Arduino Due from the platformio on MacOS via the PiTop
### 1. Ensure that the pio remote agent is running on the Pi-Top
```bash
ssh pi@<hostname>  
systemcl status pio-remote ## double check this
```
If it is not running start it & verify its status
```bash
pi@pi-top:~ $ sudo systemctl start pio-remote
pi@pi-top:~ $ sudo systemctl status pio-remote
● pio-remote.service - pio remote agent
     Loaded: loaded (/etc/systemd/system/pio-remote.service; disabled; vendor preset: enabled)
     Active: active (running) since Sun 2023-08-06 21:09:02 IST; 1s ago
   Main PID: 24017 (platformio)
      Tasks: 1 (limit: 3873)
        CPU: 1.363s
     CGroup: /system.slice/pio-remote.service
             └─24017 /usr/bin/python3 /usr/local/bin/platformio remote agent start

...
[info] Connecting to PlatformIO Remote Development Cloud
```
**NOTE**: if encounter issues with 
```bash
2023-08-06 21:24:07 [warn] Connection is lost to PIO Remote Cloud. Reconnecting
```
It is very likely the session is already started 
```bash
➜  Nasc git:(remote-development-success) pio remote agent start --name nasc &
[4] 42877
➜  Nasc git:(remote-development-success) 2023-08-06 21:24:53 [info] Name: nasc
2023-08-06 21:24:53 [info] Connecting to PlatformIO Remote Development Cloud
2023-08-06 21:24:53 [info] Successfully connected
2023-08-06 21:24:53 [info] Authenticating
2023-08-06 21:24:53 [info] Successfully authorized
2023-08-06 21:24:53 [error] PIO Remote Cloud disconnected: Agent on this host machine is already started!
2023-08-06 21:24:53 [info] Successfully disconnected

[2]    37722 done       pio remote agent start --name nasc
```

### 2. Get the port via the device list on Mac
```bash
➜ pio remote device list
2023-08-06 21:25:11 [info] Remote command received: device.list
Agent pi-top
============
/dev/ttyACM0
------------
Hardware ID: USB VID:PID=2341:003E LOCATION=1-1.1:1.0
Description: Arduino Due

/dev/ttyAMA0
------------
Hardware ID: fe201000.serial
Description: ttyAMA0

Agent nasc
==========
```

### 3. Trigger the upload from Mac
```bash 
pio remote --agent pi-top run -e due -t upload -v --upload-port /dev/ttyACM1 # port retrieved in step 2
```
**NOTE**: if this fail's build run the `build` target & retry the above
```bash
pio remote --agent pi-top run -e due -t build -v --upload-port /dev/ttyACM1
```
More issues related to bossac picking put the port...  
**Check that bossac is finding the device on the port**  
```bash
pi@pi-top:~ $ bossac --port=/dev/ttyACM1 -U -i
Device       : ATSAM3X8
Version      : v1.1 Dec 15 2010 19:25:04
Address      : 0x80000
Pages        : 2048
Page Size    : 256 bytes
Total Size   : 512KB
Planes       : 2
Lock Regions : 32
Locked       : none
Security     : false
Boot Flash   : false
```
I think a reboot of the Arduino solved it - **always use the pi-top agent**  (not sure exactly what solved this, will figure out in time)

### 4. Reset the board as that flag had to be removed to form the custom bossac command in platform.ini to work on the Pi-Top
```bash
pi@pi-top:~ $ bossac -R --port=/dev/ttyACM1
```