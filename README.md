# flipperzero-rfidbeacon
A letter/number [morse]([https://en.wikipedia.org/wiki/Letter_beacon](https://fr.wikipedia.org/wiki/Code_Morse_international)) beacon on **125 kHz** using **RFID** interface of the **Flipper Zero**

![banner](https://raw.githubusercontent.com/nmrr/flipperzero-rfidbeacon/main/img/banner-rfid.jpg)
(banner has been made with **DALL-.E 2**)


Like Russia with their HF [letter beacons](https://en.wikipedia.org/wiki/Letter_beacon), it's possible to use the **Flipper Zero** as a letter/number morse bacon. **Flipper Zero** has multiple RF interfaces (RFID, NFC and UHF) and it's possible to transmit raw data over these interfaces.

This application can transmit the whole alphabet (A to Z) and numbers (0 to 9) in morse on **RFID** interface. Range is about 5m.

Menu of the application:

<img src="https://github.com/nmrr/flipperzero-rfidbeacon/blob/main/img/off.png" width=25% height=25%> <img src="https://github.com/nmrr/flipperzero-rfidbeacon/blob/main/img/onair.png" width=25% height=25%>

**A** letter:

<img src="https://github.com/nmrr/flipperzero-rfidbeacon/blob/main/img/a.png" width=50% height=50%>

**3** number:

<img src="https://github.com/nmrr/flipperzero-rfidbeacon/blob/main/img/3.png" width=50% height=50%>

## Build the program

Assuming the toolchain is already installed, copy **flipper_rfidbeacon** directory to **applications_user**

Plug your **Flipper Zero** and build the RFID beacon:
```
./fbt launch_app APPSRC=applications_user/flipper_rfidbeacon
```

The program will automatically be launched after compilation

<img src="https://github.com/nmrr/flipperzero-rfidbeacon/blob/main/img/flipperzero.png" width=25% height=25%>

**Button assignments**: 

button  | function
------------- | -------------
**Left/Right** *[short press]* | Choose the letter/number to transmit 
**Up** *[long press]*  | Enable/disable transmitting on 125 kHz
**Back** *[long press]*  | Exit

If you don't want to build this application, just simply copy **flipper_rfidbeacon.fap** on your **Flipper Zero** 

Build has been made with official toolchain, **API Mismatch** error may appear if you are using custom firmware. You can bypass this error but the program may crash.

##What's next ?##
* Choose the speed morse code 
* Transmit on NFC interface (13.56 MHz), range is wider than **NFC**
* Transmit on UHF interface (433 MHz, etc)
