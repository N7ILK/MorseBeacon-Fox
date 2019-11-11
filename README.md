# MorseBeacon-Fox
(untested!) PCB design at https://aisler.net/p/DXDWUNGR

Simple project for interfacing inexpensive HT to Arduino Nano (clone) to make a fox hunt transmitter.

Parameters can be set remotely via DTMF for callsign, on/off,  delay period, send melody.

0: Stop transmitting. Defaults to not transmit when reset

1-9: 10-90 second between transmissions

#: Toggle sending melody (# has special meaning when entering call sign)

*: Start sending call sign in ASCII decimal - End with #. Echoes DE(call sign) on success, "0" on failure
