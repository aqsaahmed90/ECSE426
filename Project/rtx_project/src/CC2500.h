#include "stm32f4xx.h"
#include "stm32f4xx_spi.h"

/***************************************************************
 *  SmartRF Studio(tm) Export
 *
 *  Radio register settings specifed with C-code
 *  compatible #define statements.
 *
 ***************************************************************/

#ifndef SMARTRF_CC2500_H
#define SMARTRF_CC2500_H
#define CC2500_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SMARTRF_RADIO_CC2500

#define SMARTRF_SETTING_FSCTRL1    0x0C//0x12 //Frequency offset = 457kHz
#define SMARTRF_SETTING_FSCTRL0    0x00
#define SMARTRF_SETTING_FREQ2      0x5D // Carrier Frequency is 2.433GHz
#define SMARTRF_SETTING_FREQ1      0x93
#define SMARTRF_SETTING_FREQ0      0xB1
#define SMARTRF_SETTING_MDMCFG4    0x0E //0x2D // BW of channel = 541.666kHz
#define SMARTRF_SETTING_MDMCFG3    0x3B // Baud Rate = 125kb
#define SMARTRF_SETTING_MDMCFG2    0x73 //0xF3 // Disable digital DC blocking filter before demodulator, MSK modulation, 16/16 sync word bits detected
#define SMARTRF_SETTING_MDMCFG1    0x42 //0x22
#define SMARTRF_SETTING_MDMCFG0    0xF8 // Default Channel Spacing of 200kHz
#define SMARTRF_SETTING_CHANNR     0x00 // Channel 0
#define SMARTRF_SETTING_DEVIATN    0x00 //0x01 // 1785kHz
#define SMARTRF_SETTING_FREND1     0xB6
#define SMARTRF_SETTING_FREND0     0x10
#define SMARTRF_SETTING_MCSM0      0x18 // Automatically calibrate When going from IDLE to RX or TX (or FSTXON) check CC2500 datasheet
#define SMARTRF_SETTING_FOCCFG     0x1D // check datasheet
#define SMARTRF_SETTING_BSCFG      0x1C
#define SMARTRF_SETTING_AGCCTRL2   0xC7
#define SMARTRF_SETTING_AGCCTRL1   0x40 //0x00
#define SMARTRF_SETTING_AGCCTRL0   0xB0
#define SMARTRF_SETTING_FSCAL3     0xEA
#define SMARTRF_SETTING_FSCAL2     0x0A
#define SMARTRF_SETTING_FSCAL1     0x00
#define SMARTRF_SETTING_FSCAL0     0x19 //0x11
#define SMARTRF_SETTING_FSTEST     0x59
#define SMARTRF_SETTING_TEST2      0x88
#define SMARTRF_SETTING_TEST1      0x31
#define SMARTRF_SETTING_TEST0      0x0B
#define SMARTRF_SETTING_FIFOTHR    0x07
#define SMARTRF_SETTING_IOCFG2     0x29
#define SMARTRF_SETTING_IOCFG0D    0x06
#define SMARTRF_SETTING_PKTCTRL1   0x04
#define SMARTRF_SETTING_PKTCTRL0   0x05 //0x05 // Fixed Packet Length (0x05)
#define SMARTRF_SETTING_ADDR       0x00 // Global Broadcast Address
#define SMARTRF_SETTING_PKTLEN     0x0A // Packet Length of 10bytes (0xFF)

/* CONFIGURATION REGISTERS */

#define IOCFG2 0x00
#define IOCFG1 0x01
#define IOCFG0 0x02
#define FIFOTHR 0x03
#define SYNC1 0x04
#define SYNC0 0x05
#define PKTLEN 0x06
#define PKTCTRL1 0x07
#define PKTCTRL0 0x08
#define ADDR 0x09
#define CHANNR 0x0A
#define FSCTRL1 0x0B
#define FSCTRL0 0x0C
#define FREQ2 0x0D
#define FREQ1 0x0E
#define FREQ0 0x0F
#define MDMCFG4 0x10
#define MDMCFG3 0x11
#define MDMCFG2 0x12
#define MDMCFG1 0x13
#define MDMCFG0 0x14
#define DEVIATN 0x15
#define MCSM2 0x16
#define MCSM1 0x17
#define MCSM0 0x18
#define FOCCFG 0x19
#define BSCFG 0x1A
#define AGCTRL2 0x1B
#define AGCTRL1 0x1C
#define AGCTRL0 0x1D
#define WOREVT1 0x1E
#define WOREVT0 0x1F
#define WORCTRL 0x20
#define FREND1 0x21
#define FREND0 0x22
#define FSCAL3 0x23
#define FSCAL2 0x24
#define FSCAL1 0x25
#define FSCAL0 0x26
#define RCCTRL1 0x27
#define RCCTRL0 0x28
#define FSTEST 0x29
#define PTEST 0x2A
#define AGCTEST 0x2B
#define TEST2 0x2C
#define TEST1 0x2D
#define TEST0 0x2E



/* SPI RELATED DEFINITIONS */
#define CC2500_SPI                       SPI1
#define CC2500_SPI_CLK                   RCC_APB2Periph_SPI1

#define CC2500_SPI_SCK_PIN               GPIO_Pin_5                  /* PA.05 */
#define CC2500_SPI_SCK_GPIO_PORT         GPIOA                       /* GPIOA */
#define CC2500_SPI_SCK_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define CC2500_SPI_SCK_SOURCE            GPIO_PinSource5
#define CC2500_SPI_SCK_AF                GPIO_AF_SPI1

#define CC2500_SPI_MISO_PIN              GPIO_Pin_6                  /* PA.6 */
#define CC2500_SPI_MISO_GPIO_PORT        GPIOA                       /* GPIOA */
#define CC2500_SPI_MISO_GPIO_CLK         RCC_AHB1Periph_GPIOA
#define CC2500_SPI_MISO_SOURCE           GPIO_PinSource6
#define CC2500_SPI_MISO_AF               GPIO_AF_SPI1

#define CC2500_SPI_MOSI_PIN              GPIO_Pin_7                  /* PA.7 */
#define CC2500_SPI_MOSI_GPIO_PORT        GPIOA                       /* GPIOA */
#define CC2500_SPI_MOSI_GPIO_CLK         RCC_AHB1Periph_GPIOA
#define CC2500_SPI_MOSI_SOURCE           GPIO_PinSource7
#define CC2500_SPI_MOSI_AF               GPIO_AF_SPI1

#define CC2500_SPI_CS_PIN                GPIO_Pin_1                  /* PB.00 */
#define CC2500_SPI_CS_GPIO_PORT          GPIOB                       /* GPIOB */
#define CC2500_SPI_CS_GPIO_CLK           RCC_AHB1Periph_GPIOB
// #define CC2500_SPI_MISO_SOURCE           GPIO_PinSource9
// #define CC2500_SPI_MISO_AF               GPIO_AF_SPI2

// #define CC2500_SPI_INT1_PIN              GPIO_Pin_0                  /* PE.00 */
// #define CC2500_SPI_INT1_GPIO_PORT        GPIOE                       /* GPIOE */
// #define CC2500_SPI_INT1_GPIO_CLK         RCC_AHB1Periph_GPIOE
// #define CC2500_SPI_INT1_EXTI_LINE        EXTI_Line0
// #define CC2500_SPI_INT1_EXTI_PORT_SOURCE EXTI_PortSourceGPIOE
// #define CC2500_SPI_INT1_EXTI_PIN_SOURCE  EXTI_PinSource0
// #define CC2500_SPI_INT1_EXTI_IRQn        EXTI0_IRQn 

// #define CC2500_SPI_INT2_PIN              GPIO_Pin_1                  /* PE.01 */
// #define CC2500_SPI_INT2_GPIO_PORT        GPIOE                       /* GPIOE */
// #define CC2500_SPI_INT2_GPIO_CLK         RCC_AHB1Periph_GPIOE
// #define CC2500_SPI_INT2_EXTI_LINE        EXTI_Line1
// #define CC2500_SPI_INT2_EXTI_PORT_SOURCE EXTI_PortSourceGPIOE
// #define CC2500_SPI_INT2_EXTI_PIN_SOURCE  EXTI_PinSource1
// #define CC2500_SPI_INT2_EXTI_IRQn        EXTI1_IRQn 

#define CC2500_CS_LOW()       GPIO_ResetBits(CC2500_SPI_CS_GPIO_PORT, CC2500_SPI_CS_PIN)
#define CC2500_CS_HIGH()      GPIO_SetBits(CC2500_SPI_CS_GPIO_PORT, CC2500_SPI_CS_PIN)

//STROBE DEFS
#define SRES 0x30
#define SFSTXON 0x31
#define SXOFF 0x32
#define SCAL 0x33
#define SRX 0x34
#define STX 0x35
#define SIDLE 0x36
#define SWOR 0x38
#define SPWD 0x39
#define SFRX 0x3A
#define SFTX 0x3B
#endif

/* CC2500 struct */
typedef struct{
	uint8_t MISO;
	uint8_t MOSI;
	uint8_t VCC;
	uint8_t UCB0CLK;
}CC2500_InitTypeDef;
void CC2500_Init(void);
void CC2500_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
void CC2500_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
//void CC2500_LowpowerCmd(uint8_t LowPowerMode);
void CC2500_RebootCmd(void);
