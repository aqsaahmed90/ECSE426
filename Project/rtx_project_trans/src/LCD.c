#include "LCD.h"
#include "cmsis_os.h"
#include <stdio.h>
/**
 * === LCD Driver! ===
 *
 * Contrast connected to GND for max brightness
 * R/W line connected to GND since we always write to LCD
 *
 * Vss connected to GND
 * Vdd connected to +5V
 */

char bPitch [7];
char bRoll  [7];

/**
 * Initializes the GPIO pins that will be used with the LCD display
 * 
 * @param void
 * @returns void
 */

static void gpio_LCD_init(void) {
	
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable the GPIO clock */
	RCC_AHB1PeriphClockCmd(LCD_GPIO_CLOCK1, ENABLE);  
	RCC_AHB1PeriphClockCmd(LCD_GPIO_CLOCK2, ENABLE);

    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/* Rs line */
	GPIO_InitStructure.GPIO_Pin = LCD_RS;
	GPIO_Init(GPIO_PORT_RS, &GPIO_InitStructure);

	/* Enable line */
	GPIO_InitStructure.GPIO_Pin = LCD_E;
	GPIO_Init(GPIO_PORT_E, &GPIO_InitStructure);

	/* set the Enable pin to '0' */
  /* For writing to display, data is transferred only on the high to low */
  /* transition of this signal */
	GPIO_ResetBits(GPIO_PORT_E, LCD_E);

  /* Now initialize the data line, DB7 - DB0 */
  /* See the header file for the exact pin assignments */
	GPIO_InitStructure.GPIO_Pin = LCD_DB0;
	GPIO_Init(GPIO_PORT_DB0, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin = LCD_DB1;
	GPIO_Init(GPIO_PORT_DB1, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LCD_DB2;
	GPIO_Init(GPIO_PORT_DB2, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LCD_DB3;
	GPIO_Init(GPIO_PORT_DB3, &GPIO_InitStructure);  
    
 	GPIO_InitStructure.GPIO_Pin = LCD_DB4;
	GPIO_Init(GPIO_PORT_DB4, &GPIO_InitStructure);  
    
 	GPIO_InitStructure.GPIO_Pin = LCD_DB5;
	GPIO_Init(GPIO_PORT_DB5, &GPIO_InitStructure);  
    
 	GPIO_InitStructure.GPIO_Pin = LCD_DB6;
	GPIO_Init(GPIO_PORT_DB6, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Pin = LCD_DB7;
	GPIO_Init(GPIO_PORT_DB7, &GPIO_InitStructure);    
}

void LCD_init(void) {

	/* Initialize the GPIO pins first */
	gpio_LCD_init();

	/* make sure we are in 8-bit mode before we switch to nibble mode */
	/* we want to be in a reset state first */
  LCD_send_data(LCD_8_BIT_MODE, RS_COMMAND);

  /* Enable two lines display */
  LCD_send_data(LCD_TWO_LINE_ENABLE, RS_COMMAND);

	/* switch on the display */
	LCD_send_data(LCD_DISPLAY_ON, RS_COMMAND);
	LCD_clear_display();
}

void LCD_send_data(uint8_t val, uint8_t rs_line) {

  /* Set all the gpio pins on or off depending on variable val */
	GPIO_WriteBit(GPIO_PORT_DB0, LCD_DB0, BIT_ACTION(0x01 & val));

	val >>= 1;
	GPIO_WriteBit(GPIO_PORT_DB1, LCD_DB1, BIT_ACTION(0x01 & val));

	val >>= 1;
	GPIO_WriteBit(GPIO_PORT_DB2, LCD_DB2, BIT_ACTION(0x01 & val));

	val >>= 1;
	GPIO_WriteBit(GPIO_PORT_DB3, LCD_DB3, BIT_ACTION(0x01 & val)); 

    val >>= 1;
	GPIO_WriteBit(GPIO_PORT_DB4, LCD_DB4, BIT_ACTION(0x01 & val));
    
    val >>= 1;
	GPIO_WriteBit(GPIO_PORT_DB5, LCD_DB5, BIT_ACTION(0x01 & val));
    
    val >>= 1;
	GPIO_WriteBit(GPIO_PORT_DB6, LCD_DB6, BIT_ACTION(0x01 & val));
    
    val >>= 1;
	GPIO_WriteBit(GPIO_PORT_DB7, LCD_DB7, BIT_ACTION(0x01 & val));
       
	/* write the data to the display */
	LCD_enable_write(rs_line);
}

void LCD_enable_write(uint8_t rs_line) {

	/* Set the rs line to low / high based on parameter rs_line */
	/* used to tell display whether to interpret data as command or char */
	osDelay(1);
	/* if rs_line == 0, interpret data as command, else, interpret data */
	/* as characters to display */
	GPIO_WriteBit(GPIO_PORT_RS, LCD_RS, BIT_ACTION(rs_line));
    
	osDelay(1);
    
	/* Set the Enable pin high */
	GPIO_SetBits(GPIO_PORT_E, LCD_E);
	osDelay(1);
    
	/* Set the Enable pin low -- data written to lcd */
	GPIO_ResetBits(GPIO_PORT_E, LCD_E);
	osDelay(1);
}

void LCD_clear_display(void) {
	LCD_send_data(LCD_CLEAR_DISPLAY, RS_COMMAND);
}

void LCD_write_char(char *text, uint8_t length) {
	uint8_t i;

	for(i = 0; i < length; ++i) {
		/* write char to display */
		LCD_send_data(*text, RS_CHAR);

		/* increment to next char to display */
		++text;
	}
}

void LCD_move_cursor(uint8_t location) {
	LCD_send_data(LCD_LOCATION_COMMAND | location, RS_COMMAND);
}


void LCD_move_second_line(void) {
    LCD_move_cursor(40);
}


void LCD_write_angle(angle_data angle){

		snprintf ( bPitch, sizeof(bPitch), "%f",angle.pitch);
		snprintf ( bRoll,  sizeof(bRoll), "%f" ,angle.roll);
	
		LCD_write_char("Pitch: ", 7);
		LCD_write_char(bPitch,  6);
		
		LCD_move_second_line();
		
		LCD_write_char("Roll : ",  7);
		LCD_write_char(bRoll,  6);
		LCD_move_cursor(0);
		osDelay(100);
}
