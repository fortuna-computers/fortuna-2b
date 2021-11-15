Boards:

 - Main board
   - External connections
     - USB power
     - Power button
     - Indicator lights
       - Power
       - Storage activity
     - SDCard
     - PS/2 keyboard
     - VGA
     - Serial (UART) / USB (FTDI) ?
   - Connector for auxiliary boards
   - ICs
     - Z80
     - Flip/flop for WAIT line
     
 - I/O board
   - Functionalities
     - UART
     - ROM
     - SDCard + FAT
     - Crystal
     - Clock for the whole system
   - ICs
     - ATMEGA16 (or ATMEGA328?)
     - 74HC595 for Address bus expansion
     - RTC?
     - Crystal?
   - Header for Fortuna Connector
   - How to select the clock?

 - RAM board
   - ICs
     - 62256 x2
     - 74HC00 to select which memory bank

 - Video/keyboard board
   - Use fortuna-terminal
