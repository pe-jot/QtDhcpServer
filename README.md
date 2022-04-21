## Introduction

QtCIV is a [Qt](https://www.qt.io/) C++ implementation of ICOM's CI-V protocol.

Altough primarily developed on Windows platform, it is cross-platform compatible as it is using Qt framework.

The applications provided are mainly intended to provide some basic functionality to communicate with [ICOM IC-705](https://www.icomjapan.com/lineup/products/IC-705/) amateur radio transceiver.
Similar devices like IC-7300, IC-9700,... may be supported as well. Please cross-check with the corresponding [CI-V reference manuals](https://www.icomjapan.com/support/manual/).

## Project organization

### libCIV
Library containing basic CI-V protocol functionality.

### libCIVTest
Qt Unit test project for libCIV.

### RemoteControl
A Qt QML based demo project making use of most of the functionality currently implemented in libCIV.  
![Screenshot RemoteControl GUI](RemoteControl.png)

### SatSync
A Qt Widgets based project used to synchronize transmit frequency corresponding to receive frequency shift when making satellite communication (compensating Doppler shift).

The intended usecase is that one tunes to the exact receiving frequency on the transmitter and the software calculates the correct transmit frequency and sets it then on the device.
The transmit frequency is being calculated as follows:  
![Doppler shift formula](DopplerShiftFormula.png)

Windows GUI (with transmitter connected):  
![Screenshot SatSync GUI](SatSync.png)

Linux / Ubuntu GUI (no connection to transceiver):  
![Screenshot SatSync GUI Ubuntu](SatSync_Ubuntu.png)

### CIVBridge
Please see [RemoteTRX](https://github.com/pe-jot/RemoteTRX) for details.