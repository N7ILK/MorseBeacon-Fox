# MorseBeacon-Fox

 Simple project for interfacing NiceRF SA828 to Arduino Nano (clone) to make a fox hunt transmitter.

Hardware:
Battery connects to both SA828 and vin of Arduino. 100mfd across battery suppresses spikes when xmitting - 
Pin 6 to MIC+ - 
Pin 7 to PTT - 
Pin A0 to SPK closest to edge of board with antenna connector - 
Jumper H/L to GND for .5W output. This was plenty for our hunt within a wildlife refuge - 
250K ohm variable across VOL pins. I'm set a bit less than halfway ~160k ohms

You'll need the software to program your frequency/ies into the SA828. It's available on the Nice RF website

Parameters can be set remotely via DTMF for callsign, on/off,  delay period, send melody.

0: Stop transmitting. Defaults to not transmit when reset

1-9: 10-90 second between transmissions

#: Toggle sending melody (# has special meaning when entering call sign)

*: Start sending call sign in ASCII decimal - End with #. Echoes DE(call sign) on success, "0" on failure
