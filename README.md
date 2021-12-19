# gnuradio-dpdk
UDP RX implementation for IQ samples using DPDK

# Features
* accepts UDP packets with legacy CHDR header (64 bits) and IQ samples in LE format
* outputs complex stream

For details of legacy CHDR see https://files.ettus.com/manual/page_rtp.html

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
![GRC](/screenshots/grc.png)
* run the generated script as root
* start the trex data generator (see https://trex-tgn.cisco.com/trex/doc/trex_stateless.html#_getting_started_tutorials for details)
```
sudo ./t-rex-64 -i
```
in a different terminal:
```
./trex-console
start -f chdr_generator.py
```
![output](/screenshots/rx.png)

# Todo
* C++ unit tests
* check seqnum of CHDR
* add more packet formats and output types
* add TX (sink)
