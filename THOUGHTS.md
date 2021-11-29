# IO Requests

I/O requests are always done by writing to the I/O port `0x0`. If there is return data, it'll be 
written directly to RAM.

The RAM addresses used are:

| Address      | Register     | Description | Size |
|--------------|--------------|-------------|------|
| `0100`       | `I_CMD`      | Request command | 8 bits |
| `0101`       | `I_STATUS`   | Last command status | 8 bits |
| `0102..0103` | `I_ORIG`     | Origin of data in RAM | 16 bits |
| `0104..0105` | `I_DEST`     | Destination of data in RAM | 16 bits |
| `0106..0109` | `I_SD_BLOCK` | Block of SD card being operated on | 32 bits |
| `010A..010B` | `I_SZ`       | Operation size | 16 bits |
| `0200..03FF` | Buffer used to exchange 512-byte blocks of data | Read/write | 512 bytes |

I/O requests:

| Request               | Byte   | Request parameters | Description |
|-----------------------|--------|--------------------|----------|
| `I_LAST_KEYPRESS`     | `0x00` | `I_DEST`           | Store last keypress in memory location (0 = no keypress) |
| `I_PRINT`             | `0x01` | `I_ORIG`           | Print char to screen |
| `I_SDCARD_RAW_READ`   | `0x02` | `I_DEST`, `I_SD_BLOCK` | Read a SDCard block and store it on RAM |
| `I_SDCARD_RAW_WRITE`  | `0x03` | `I_ORIG`, `I_SD_BLOCK` | Write a SDCard block from RAM |
| `I_RANDOM`            | `0x04` | `I_DEST`, `I_SZ` (1 to 4) | Store random value in memory location |
| `I_MEMCPY`            | `0x05` | `I_ORIG`, `I_DEST`, `I_SZ` | Copy a memory block from one location to the other |
| `I_RTC_GET`           | `0x06` |                    | Store RTC values in buffer (`yy mm dd hh nn ss`) |
| `I_RTC_SET`           | `0x07` |                    | Set RTC based on buffer values (`yy mm dd hh nn ss`) |
| `I_POST_TEST`         | `0xff` | `I_DEST`           | Set memory location with a predefined value. Used by POST only. |
