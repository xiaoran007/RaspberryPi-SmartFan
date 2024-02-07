# RaspberryPi-SmartFan
Modified smart cooling fan module driver for raspberry Pi 3B and compatible Dev boards


## Compatibility
This driver is write for raspberry Pi 3B, compatible with RaspiOS (raspbian) system. Since WiringPi library is no longer maintained, so this driver may not work properly on newer versions of the system.

Tested on following platforms:

| OS   | Arch | Kernel | Base |
| :---: | :---: | :---: | :---: |
| Raspberry Pi OS (Legacy) |   32-bit   | 6.1 | Debian 11 (bullseye) |
| Raspberry Pi OS (Legacy) |   64-bit   | 6.1 | Debian 11 (bullseye) |

## Build from source
### Requirements
The following packages are required
* git
* gcc
* make
### Prepare
Enable Raspberry Pi I2C interface:
```shell
sudo raspi-config
```
select Interfacing Options - I2C Enabled

Then make sure WiringPi library is installed, check:
```shell
gpio -v
```
If not, build and install from source:
```shell
git clone https://github.com/WiringPi/WiringPi.git
cd ./WiringPi
sudo ./build
```
### Build
use make to build this module:
```shell
make main
```
### Install
you can start driver manually, or use following command to set auto start with the system:
```shell
sudo make install
```

## Auto start setting
check if rc-local service is enabled:
```shell
systemctl status rc-local
```
if enabled then:

edit rc.local file, insert the path to the executable file before "exit 0":
```shell
sudo vim /etc/rc.local
```
example path:
```shell
~/RaspberryPi-SmartFan/build/smart_fan
```
rc.local service may not enable in some system's default settings, use following commands to enable this service:
```shell
chmod +x /etc/rc.local
systemctl daemon-reload
systemctl start rc-local
systemctl status rc-local
```
You can reboot now.