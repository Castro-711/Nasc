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

## How to upload new code to the Arduino Due from the platformio on MacOS via the PiTop
#### 1. Ensure that the pio remote agent is running on the rpi
```bash
ssh pi@<hostname>  
systemcl status pio-remote ## double check this
```

#### 2. Get the port via the device list
```bash
pio remote device list
```

#### 3. Trigger the upload 
```bash 
pio remote --agent pi-top run -e due -t upload -v --upload-port /dev/ttyACM1 # port retrieved in step 2
```
**NOTE**: if this fail's build run the `build` target & retry the above
```bash
pio remote --agent pi-top run -e due -t build -v --upload-port /dev/ttyACM1
```

#### 4. Reset the board as that step had to be removed form the bossac command
```bash
bossac --port=<port-name> -i -R ## something like that - double check
```