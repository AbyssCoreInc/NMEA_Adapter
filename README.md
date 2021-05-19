# NMEA0183 - Fiware Adapter

Purpose of this piece of software is to capture NMEA0183 traffic using serial adapter like that by Brookhouse and
inject that Fiware using MQTT Ultra Light protocoll. Datamodel for NMEA0183 devices is somethign that is crucial and 
somewhat use case specific. Simple approach is to have each talker identifier or sentence identifier as its own entity 
and all datafields as attributes to it.

## End Goal
End goal is to build adapter to OpenCPN that can use Fiware as data source for ship data. To this end data need to be 
structured in more logical way than simple devices and data they produce. This service is a component to that offers 
access to legacy devices that use NMEA0183 protocoll. Other components are build for NMEA2000 devices and other 
generic or proprietary data sources like Victron power systems that use VE.Direct or CanBUS bus. 

## Dependencies
Following dependencies need to be installed to compile the software
```
sudo apt-get install -y nlohmann-json-dev

# libusbp
sudo apt-get install cmake libudev-dev
git clone https://github.com/pololu/libusbp.git
cd libusbp
mkdir build
cd build
cmake ..
make
sudo make install
```
