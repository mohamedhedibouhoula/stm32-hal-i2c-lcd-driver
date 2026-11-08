#include "lcd.h"

#define LCD_ADDR  (0x27 << 1)   // I2C address (shifted for HAL)
#define LCD_BL    0x08          // bit 3: backlight
#define LCD_EN    0x04          // bit 2: enable
#define LCD_RS    0x01          // bit 0: 0 = command, 1 = character

static I2C_HandleTypeDef *lcd_i2c;   // the I2C bus, saved in lcd_init()

static void lcd_write_byte(uint8_t b)
{
  HAL_I2C_Master_Transmit(lcd_i2c, LCD_ADDR, &b, 1, 100);
}

static void lcd_pulse(uint8_t nibble, uint8_t rs)
{
  uint8_t b = (nibble & 0xF0) | LCD_BL | rs;
  lcd_write_byte(b | LCD_EN);   // EN = 1
  HAL_Delay(1);
  lcd_write_byte(b);            // EN = 0 -> LCD reads here
  HAL_Delay(1);
}

static void lcd_send_cmd(uint8_t cmd)
{
  lcd_pulse(cmd & 0xF0, 0);
  lcd_pulse((cmd << 4) & 0xF0, 0);
}

static void lcd_send_data(uint8_t data)
{
  lcd_pulse(data & 0xF0, LCD_RS);
  lcd_pulse((data << 4) & 0xF0, LCD_RS);
}

void lcd_init(I2C_HandleTypeDef *hi2c)
{
  lcd_i2c = hi2c;               // remember which I2C to use

  HAL_Delay(50);
  lcd_pulse(0x30, 0); HAL_Delay(5);    // 8-bit mode: single pulses
  lcd_pulse(0x30, 0); HAL_Delay(1);
  lcd_pulse(0x30, 0); HAL_Delay(10);
  lcd_pulse(0x20, 0); HAL_Delay(10);   // switch to 4-bit mode

  lcd_send_cmd(0x28);   // 4-bit, 2 lines, 5x8 font
  lcd_send_cmd(0x08);   // display OFF
  lcd_clear();
  lcd_send_cmd(0x06);   // cursor moves right
  lcd_send_cmd(0x0C);   // display ON, cursor OFF
}

void lcd_clear(void)
{
  lcd_send_cmd(0x01);
  HAL_Delay(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col)
{
  lcd_send_cmd((row == 0 ? 0x80 : 0xC0) + col);
}

void lcd_print(const char *str)
{
  while (*str)
  {
    lcd_send_data(*str++);
  }
}
