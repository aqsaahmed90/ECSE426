#ifndef __HD44780_H
#define __HD44780_H

#include <inttypes.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "ACCEL.h"

#define LCD_GPIO_CLOCK1 	RCC_AHB1Periph_GPIOE
#define LCD_GPIO_CLOCK2 	RCC_AHB1Periph_GPIOB


#define LCD_RS					GPIO_Pin_8
#define GPIO_PORT_RS		GPIOB

#define LCD_E 					GPIO_Pin_9
#define GPIO_PORT_E		  GPIOB	

#define LCD_DB0					GPIO_Pin_0
#define GPIO_PORT_DB0		GPIOE

#define LCD_DB1					GPIO_Pin_1
#define GPIO_PORT_DB1		GPIOE

#define LCD_DB2					GPIO_Pin_2
#define GPIO_PORT_DB2		GPIOE

#define LCD_DB3					GPIO_Pin_3
#define GPIO_PORT_DB3		GPIOE

#define LCD_DB4					GPIO_Pin_4
#define GPIO_PORT_DB4		GPIOE

#define LCD_DB5					GPIO_Pin_5
#define GPIO_PORT_DB5		GPIOE

#define LCD_DB6					GPIO_Pin_6
#define GPIO_PORT_DB6		GPIOE

#define LCD_DB7					GPIO_Pin_7
#define GPIO_PORT_DB7		GPIOB

#define RS_COMMAND					0
#define RS_CHAR							1


#define LCD_TWO_LINE_ENABLE		    	0x38
#define LCD_8_BIT_MODE              0x30

#define LCD_DISPLAY_ON							0x0C
#define LCD_CURSOR_UNDERLINE        0x02
#define LCD_CURSOR_BLINK 						0x01

#define LCD_CLEAR_DISPLAY						0x01
#define LCD_LOCATION_COMMAND        0x80
/* function to return the right enum if VAL == 0 or VAL == 1 */
/* function needed to remove warning when GPIO_WriteBit expects an enum */
/* whereas an int is provided instead */

#define BIT_ACTION(VAL) ((VAL) == 0 ? Bit_RESET: Bit_SET)

/**
 * Initialize the display
 *
 * - Sets up GPIO pins (PE3-PE8)
 * - Initialize nibble mode
 * - Enables the display
 *
 * @param void
 * @returns void
 */
void LCD_init(void);


/**
 * @param val: value to write to display register
 * @param rs_line: either RS_COMMAND or RS_CHAR
 *
 * @returns void
 */

void LCD_send_data(uint8_t val, uint8_t rs_line);

/**
 * Sets the enable line to low, then to high again
 *
 * Doing so makes the data in line D7-D4 be written to the register of the
 * display.
 * Depending on rs_line, the RS GPIO will be set to '1' or '0'
 *
 * @param rs_line: either RS_COMMAND or RS_CHAR
 * @returns void
 */
void LCD_enable_write(uint8_t rs_line);

/**
 * Clears the display -- erases any text on the display
 *
 * @param void
 * @returns void
 */
void LCD_clear_display(void);

/**
 * Write characters to the display (maybe modify it to consider cursor position)
 *
 * @param text: String to write to display
 * @param length: length of text to print to display
 *
 * @returns void
 */
void LCD_write_char(char *text, uint8_t length);

/**
 * Move the cursor to location specified in parameter
 *
 * @param location: location to move cursor to
 * @returns void
 */
void LCD_move_cursor(uint8_t location);

/**
 * @brief Move cursor on LCD to second line
 *
 * @param  void.
 * @return void.
 */
void LCD_move_second_line(void);

/**
 * @brief Writes the specified angle data to the LCD.
 *
 * @param angle: The angle that the motors are moving to.
 */
void LCD_write_angle(angle_data angle);
#endif
