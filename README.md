# RaspberryPi-SmartFan
Modified smart cooling fan module driver for raspberry Pi 3B and compatible Dev boards

## Build from source
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
Then use make to build this module:
```shell
make
```