Boards:

 - Main board
   - External connections
     - USB power
     - Power button
     - Indicator LEDs
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
     - Memory copy block
     - Random value provider
     - Wall clock
     - Clock signal for the whole system
   - ICs
     - ATMEGA16
       - Pins: 5V, GND, Reset, UART (2), 595 control (4), DATA (8), clock out, memory control (3), SDCard (4), IORQ (interrupted), BUSRQ/AK (2), RSTZ80, WAITST
       - Maybe: interrupt, NMI, I²C for RTC (2), crystal (2)
       - Total: 30, maybe 6
     - 74HC595 for Address bus expansion
       - Pins controlling 74HC595: SER (x2), SRCLK, OE (RCLK and SRCLR are kept high)
     - RTC?
     - Crystal?
   - Header for Fortuna Connector
   - How to select the clock?
   - Pinout: 
     - 5V, GND, ADDR (16), DATA (8), UART (2), SDCard (4), Clock, MREQ, IORQ, RD, WR, BUSRQ, BUSAK, RSTZ80, WAITST
     - Maybe: INT, NMI
     - Total: 43 pins (at least)

 - RAM board
   - ICs
     - 62256 x2
     - 74HC00 to select which memory bank
   - Pinout:
     - 5V, GND, ADDR (16), DATA (8), MREQ, RD, WR
     - Total: 29 pins (at least)
 
 - Video/keyboard board
   - Use fortuna-terminal

 - Maybe external connection to debugger?

