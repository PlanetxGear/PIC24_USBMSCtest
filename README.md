﻿# PIC24FJ64GB002 USB MSC Class Test Project.
---
## Description

This is a simple USB MSC Class Test Project with [PIC24FJ64GB002 micro controller][5] & [MPLAB.X][1] & [MCC][2].
I don't use [MLA][6](Microchip Livraries for Applications). I directly manipulate the USB registers.
You can operat it by terminal with serial communication speed of 115,200 bps.

## Picture
![Alt text](/Picture/20190822_IMG_2250.JPG)


## References.
<http://ww1.microchip.com/downloads/en/DeviceDoc/39721b.pdf>
<http://ww1.microchip.com/downloads/jp/DeviceDoc/39721B_JP.pdf>
<http://www.picfun.com/usb03.html>


## Prerequisites
I created this project using the free versions of the following tools:
 * [Microchip MPLAB X][1] (v5.20)
 * [Microchip MPLAB X Code Configurator][2] (v3.85)
 * [Microchip XC16 Compiler][4] (v1.36)
 * [KiCad][6] (v4.0.1)

You shoud download and install these tools appropriately before you will open this project and build it.
I use the [Microchip MPLAB PICkit3 In-Circuit Debugger][3] to program the PIC and debug it.  

Also, I use the [KiCad][6] to draw the electric circuit schematic.


## Licence
[MIT]<https://en.wikipedia.org/wiki/MIT_License> <https://ja.wikipedia.org/wiki/MIT_License>


## Author
Hiroshi Murakami  
<https://www.planetxgear.com/>  

[1]: http://www.microchip.com/pagehandler/en-us/family/mplabx/ "MPLAB X"
[2]: http://www.microchip.com/mplab/mplab-code-configurator "MPLAB Code Configurator "
[3]: http://www.microchip.com/Developmenttools/ProductDetails.aspx?PartNO=PG164130 "MPLAB PICkit3 In-Circuit Debugger"
[4]: http://www.microchip.com/pagehandler/en_us/devtools/mplabxc/ "MPLAB XC Compilers"
[5]: https://www.microchip.com/wwwproducts/en/PIC24FJ64GB002
[6]: https://www.microchip.com/mplab/microchip-libraries-for-applications 

