# STM32 HAL I2C LCD Driver

A small driver for a 16x2 character LCD with a PCF8574 I2C backpack, written in C using only the STM32 HAL. It uses no Arduino libraries.

It was built and tested on an **STM32C031C6 (NUCLEO-C031C6)** with STM32CubeIDE and runs in the Wokwi simulator.

## Features

- Pure STM32 HAL (`HAL_I2C_Master_Transmit`), so it works in any STM32CubeIDE project.
- Runs the LCD in 4-bit mode through the PCF8574 I2C expander.
- Small public API: `init`, `clear`, `set_cursor`, `print`.
- The I2C handle is passed in at init, so the driver isn't tied to `I2C1`.
- Low-level helpers are kept `static` (private) inside `lcd.c`.

## Project structure

```
Inc/lcd.h        Public API
Src/lcd.c        Driver implementation
example/main.c   Example: shows LED ON / LED OFF in sync with the board LED
```

## Wiring (NUCLEO-C031C6)

| LCD (PCF8574) | Nucleo         |
|---------------|----------------|
| GND           | GND            |
| VCC           | 5V             |
| SDA           | PB9 (D14)      |
| SCL           | PB8 (D15)      |

I2C address: `0x27`

## How to use

1. In STM32CubeMX, enable **I2C1** (Standard Mode, 100 kHz) on PB8/PB9.
2. Copy `lcd.h` into `Core/Inc` and `lcd.c` into `Core/Src`.
3. In `main.c`:

```c
#include "lcd.h"

lcd_init(&hi2c1);
lcd_set_cursor(0, 0);
lcd_print("Hello Hedi");
```

## API

| Function | Description |
|----------|-------------|
| `void lcd_init(I2C_HandleTypeDef *hi2c)` | Starts the LCD in 4-bit mode on the given I2C bus |
| `void lcd_clear(void)` | Clears the display |
| `void lcd_set_cursor(uint8_t row, uint8_t col)` | Moves the cursor (row 0–1, col 0–15) |
| `void lcd_print(const char *str)` | Prints a string at the cursor |

## How it works

Each byte sent to the PCF8574 maps to the LCD pins:

| Bit | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0  |
|-----|----|----|----|----|----|----|----|----|
| Pin | D7 | D6 | D5 | D4 | BL | EN | RW | RS |

- Each character or command is sent as two 4-bit nibbles.
- Each nibble is latched by an **EN pulse**: one byte with EN=1, then one with EN=0. The LCD reads on the falling edge.
- The init sequence sends single nibbles (`0x3`, `0x3`, `0x3`, `0x2`) while the LCD is still in 8-bit mode, then switches to 4-bit mode.

## Lessons learned

- **Nibble alignment matters.** Sending full two-nibble commands during the 8-bit init phase shifted every later command by half a byte, and the display stayed blank.
- **Never `#include` a `.c` file.** One wrong include caused a recursive include and more than 2,000 compiler errors.

## License

MIT
