# gnuradio-dpdk
UDP RX implementation for IQ samples using DPDK

# Features
* Accepts UDP packets with legacy CHDR header (64 bits) and IQ samples in LE format
* outputs complex stream

#  Requirements
* pkg-config (tested with Ubuntu 21.04)
* dpdk (tested with 21.11)
* gnuradio >= 3.8
* cmake
* cisco trex for testing

# Installation
```
mkdir build
cd build
cmake ../
```
Specify the python site-package path if multiple python installations are available:
```
cmake ../ -DGR_PYTHON_DIR=/usr/local/lib/python3.9/dist-packages
```
```
make
make install
```

# Test
* setup DPDK and bind interface
* wire up a dpdk_source (port 12) to a QT GUI time sink (autoscale enabled)
* run the generated script as root
* start the trex data generator
```
start -f chdr_generator.py
```
Expected output on the time sink


#Todo
* C++ unit tests
