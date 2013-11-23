#include "CC2500.h"
#include "cmsis_os.h"



__IO uint32_t  CC2500Timeout = CC2500_FLAG_TIMEOUT; 
/* Read/Write command */
#define READWRITE_CMD              ((uint8_t)0x80) 
/* Multiple byte read/write command */ 
#define MULTIPLEBYTE_CMD           ((uint8_t)0x40)
/* Dummy Byte Send by the SPI Master device in order to generate the Clock to the Slave device */
#define DUMMY_BYTE                 ((uint8_t)0x00)

uint8_t CC2500_state;

static uint8_t CC2500_SendByte(uint8_t byte);
static void CC2500_LowLevel_Init(void);
uint8_t CC2500_Strobe(uint8_t Strobe);

void delay(long num_ticks)
{
	while(num_ticks-- > 0);
}

uint8_t CC2500_CheckRegisters(uint8_t* pBuffer, uint8_t TestAddr, uint16_t NumByteToTest){
	uint8_t ctrl[16];
  int i = 0;
	CC2500_Read(ctrl, TestAddr, NumByteToTest);
	
	while(i < NumByteToTest){
		if(pBuffer[i] != ctrl[i]) return 0;
		i++;
	}
	return 1;
	
}

void CC2500_Init(void){
	uint8_t ctrl[16];
	//Configure the low level interface ----------------------------
	CC2500_LowLevel_Init();
	
// 	//Do SRES Strobe to reset chip
// 	
// 	GPIO_WriteBit(CC2500_SPI_SCK_GPIO_PORT, CC2500_SPI_SCK_PIN, 1);
// 	GPIO_WriteBit(CC2500_SPI_MOSI_GPIO_PORT, CC2500_SPI_MOSI_PIN, 0);
// 	
// 	CC2500_CS_LOW();
// 	CC2500_CS_HIGH();
// 	osDelay(1);
// 	CC2500_CS_LOW();
// 	//Wait until SO goes low
// 	
// 	CC2500_Strobe(SRES);
	
	//TUESDAY
	ctrl[0] = SMARTRF_SETTING_IOCFG2;
	CC2500_Write(ctrl, IOCFG2, 1);
	CC2500_Write(ctrl, IOCFG2, 1);
	while(!CC2500_CheckRegisters(ctrl, IOCFG2, 1));
	
	ctrl[0] = SMARTRF_SETTING_IOCFG0D;
	ctrl[1] = SMARTRF_SETTING_FIFOTHR;
	CC2500_Write(ctrl, IOCFG0, 2);
	while(!CC2500_CheckRegisters(ctrl, IOCFG0, 2))CC2500_Write(ctrl, IOCFG0, 2);
	
	ctrl[0]  = SMARTRF_SETTING_PKTLEN;
	ctrl[1]  = SMARTRF_SETTING_PKTCTRL1;
	ctrl[2]  = SMARTRF_SETTING_PKTCTRL0;
	ctrl[3]  = SMARTRF_SETTING_ADDR;
	ctrl[4]  = SMARTRF_SETTING_CHANNR;
	ctrl[5]  = SMARTRF_SETTING_FSCTRL1;
	ctrl[6]  = SMARTRF_SETTING_FSCTRL0;
	ctrl[7]  = SMARTRF_SETTING_FREQ2;
	ctrl[8]  = SMARTRF_SETTING_FREQ1;
	ctrl[9]  = SMARTRF_SETTING_FREQ0;
	ctrl[10] = SMARTRF_SETTING_MDMCFG4;
	ctrl[11] = SMARTRF_SETTING_MDMCFG3;
	ctrl[12] = SMARTRF_SETTING_MDMCFG2;
	ctrl[13] = SMARTRF_SETTING_MDMCFG1;
	ctrl[14] = SMARTRF_SETTING_MDMCFG0;
	ctrl[15] = SMARTRF_SETTING_DEVIATN;
	CC2500_Write(ctrl, PKTLEN, 16);
	while(!CC2500_CheckRegisters(ctrl, PKTLEN, 16))CC2500_Write(ctrl, PKTLEN, 16);;
	
	ctrl[0] = SMARTRF_SETTING_MCSM0;
	ctrl[1] = SMARTRF_SETTING_FOCCFG;
	ctrl[2] = SMARTRF_SETTING_BSCFG;
	ctrl[3] = SMARTRF_SETTING_AGCCTRL2;
	ctrl[4] = SMARTRF_SETTING_AGCCTRL1;
	ctrl[5] = SMARTRF_SETTING_AGCCTRL0;
	CC2500_Write(ctrl, MCSM0, 6);
	while(!CC2500_CheckRegisters(ctrl, MCSM0, 6))CC2500_Write(ctrl, MCSM0, 6);;
	
	ctrl[0] = SMARTRF_SETTING_FREND1;
	ctrl[1] = SMARTRF_SETTING_FREND0;
	ctrl[2] = SMARTRF_SETTING_FSCAL3;
	ctrl[3] = SMARTRF_SETTING_FSCAL2;
	ctrl[4] = SMARTRF_SETTING_FSCAL1;
	ctrl[5] = SMARTRF_SETTING_FSCAL0;
	ctrl[5] = SMARTRF_SETTING_FSTEST;
	CC2500_Write(ctrl, FREND1, 7);
	while(!CC2500_CheckRegisters(ctrl, FREND1, 7))CC2500_Write(ctrl, FREND1, 7);;
	
	ctrl[0] = SMARTRF_SETTING_TEST2;
	ctrl[1] = SMARTRF_SETTING_TEST1;
	ctrl[2] = SMARTRF_SETTING_TEST0;
	CC2500_Write(ctrl, TEST2, 3);
	while(!CC2500_CheckRegisters(ctrl, TEST2, 3))CC2500_Write(ctrl, TEST2, 3);;
}

void CC2500_LowLevel_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
	uint8_t ctrl;

  /* Enable the SPI periph */
  RCC_APB2PeriphClockCmd(CC2500_SPI_CLK, ENABLE);

  /* Enable SCK, MOSI and MISO GPIO clocks */
  RCC_AHB1PeriphClockCmd(CC2500_SPI_SCK_GPIO_CLK | CC2500_SPI_MISO_GPIO_CLK | CC2500_SPI_MOSI_GPIO_CLK, ENABLE);

  /* Enable CS  GPIO clock */
  RCC_AHB1PeriphClockCmd(CC2500_SPI_CS_GPIO_CLK, ENABLE);

  GPIO_PinAFConfig(CC2500_SPI_SCK_GPIO_PORT, CC2500_SPI_SCK_SOURCE, CC2500_SPI_SCK_AF);
  GPIO_PinAFConfig(CC2500_SPI_MISO_GPIO_PORT, CC2500_SPI_MISO_SOURCE, CC2500_SPI_MISO_AF);
  GPIO_PinAFConfig(CC2500_SPI_MOSI_GPIO_PORT, CC2500_SPI_MOSI_SOURCE, CC2500_SPI_MOSI_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  /* SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_SCK_PIN;
  GPIO_Init(CC2500_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /* SPI  MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  CC2500_SPI_MOSI_PIN;
  GPIO_Init(CC2500_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /* SPI MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_MISO_PIN;
  GPIO_Init(CC2500_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /* SPI configuration -------------------------------------------------------*/
  SPI_I2S_DeInit(CC2500_SPI);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(CC2500_SPI, &SPI_InitStructure);

  /* Enable SPI1  */
  SPI_Cmd(CC2500_SPI, ENABLE);

  /* Configure GPIO PIN for Lis Chip select */
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(CC2500_SPI_CS_GPIO_PORT, &GPIO_InitStructure);

  /* Deselect : Chip Select high */
  GPIO_SetBits(CC2500_SPI_CS_GPIO_PORT, CC2500_SPI_CS_PIN);
	
	//CC2500_Read(&ctrl, 0x30, 1);
	CC2500_CS_LOW();
	delay(100);
	CC2500_CS_HIGH();
	delay(100);
	CC2500_CS_LOW();
	delay(150);
	
	// Send reset command
	CC2500_Strobe(SRES); 
	while(!GPIO_ReadInputDataBit(CC2500_SPI_MISO_GPIO_PORT, CC2500_SPI_MISO_PIN));
	CC2500_CS_HIGH();

	// Set to IDLE state
	CC2500_Strobe(SIDLE);
}

/**
  * @brief  Sends a Byte through the SPI interface and return the Byte received 
  *         from the SPI bus.
  * @param  Byte : Byte send.
  * @retval The received byte value
  */
static uint8_t CC2500_SendByte(uint8_t byte)
{
  /* Loop while DR register in not emplty */
  CC2500Timeout = CC2500_FLAG_TIMEOUT;
  while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_TXE) == RESET)
  {
//     if((CC2500Timeout--) == 0) return 0;
  }
  
  /* Send a Byte through the SPI peripheral */
  SPI_I2S_SendData(CC2500_SPI, byte);
  
  /* Wait to receive a Byte */
  CC2500Timeout = CC2500_FLAG_TIMEOUT;
  while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_RXNE) == RESET)
  {
//     if((CC2500Timeout--) == 0) return 0;
  }
  
  /* Return the Byte read from the SPI bus */
  return (uint8_t)SPI_I2S_ReceiveData(CC2500_SPI);
}

/**
  * @brief  Writes one byte to the CC2500.
  * @param  pBuffer : pointer to the buffer  containing the data to be written to the CC2500.
  * @param  WriteAddr : CC2500's internal address to write to.
  * @param  NumByteToWrite: Number of bytes to write.
  * @retval None
  */
void CC2500_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
	/* Configure the Burst mode bit: 
    - When 0, the address will remain unchanged in multiple read/write commands.
    - When 1, the address will be auto incremented in multiple read/write commands.
    */  
    if(NumByteToWrite > 0x01)
    {
        // Set Burst mode bit
        WriteAddr |= (uint8_t)MULTIPLEBYTE_CMD;
    }
    
    // Set chip select Low at the start of the transmission
    CC2500_CS_LOW(); 

    
    // Send the Address of the indexed register
    // and read the state of the CC2500 peripheral.
    CC2500_state = CC2500_SendByte(WriteAddr);
    
    // Send the data that will be written into the device (MSB First)
    while(NumByteToWrite >= 0x01)
    {
        CC2500_SendByte(*pBuffer);
        NumByteToWrite--;
        pBuffer++;
    }

    // Set chip select High at the end of the transmission
    CC2500_CS_HIGH();
}

/**
  * @brief  Reads a block of data from the CC2500.
  * @param  pBuffer : pointer to the buffer that receives the data read from the CC2500.
  * @param  ReadAddr : CC2500's internal address to read from.
  * @param  NumByteToRead : number of bytes to read from the CC2500.
  * @retval None
  */
void CC2500_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{  
  if(NumByteToRead > 0x01)
  {
    ReadAddr |= (uint8_t)(READWRITE_CMD | MULTIPLEBYTE_CMD);
  }
  else
  {
    ReadAddr |= (uint8_t)READWRITE_CMD;
  }
  /* Set chip select Low at the start of the transmission */
  CC2500_CS_LOW();
  
  /* Send the Address of the indexed register */
  CC2500_SendByte(ReadAddr);
  
  /* Receive the data that will be read from the device (MSB First) */
  while(NumByteToRead > 0x00)
  {
    /* Send dummy byte (0x00) to generate the SPI clock to CC2500 (Slave device) */
    *pBuffer = CC2500_SendByte(DUMMY_BYTE);
    NumByteToRead--;
    pBuffer++;
  }
  
  /* Set chip select High at the end of the transmission */ 
  CC2500_CS_HIGH();
}

uint8_t CC2500_Strobe(uint8_t Strobe){
// 	uint8_t *buff;
// 	CC2500_CS_LOW();
// 	CC2500_SendByte(Strobe);
// 	//Wait until SO goes high
// 	CC2500_Read(buff, 0xBF, 0);
// 	CC2500_CS_HIGH();
	
	CC2500_CS_LOW();

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)== RESET);	// check flag for transmission to be RESET
	SPI_I2S_SendData(SPI1, Strobe);												// condition satisfied --> send command strobe

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);		// check flag for being busy to be SET
	CC2500_state = (SPI_I2S_ReceiveData(SPI1) & 0x70) >> 4;											// set status to most recent received data on SPI1

	// Set chip select High at the end of the transmission
	CC2500_CS_HIGH();   
	return CC2500_state;
}

uint8_t* CC2500_RXData(void){
	uint8_t bytes_in_rxfifo;
	uint8_t data[4];
	CC2500_Strobe(SRX);
	
	while(CC2500_Strobe(SNOP) != 1);
	
	CC2500_Read(&bytes_in_rxfifo, RXBYTES, 1);
	bytes_in_rxfifo = bytes_in_rxfifo & 0x7F;
	if(bytes_in_rxfifo >= 4){
		CC2500_Read(data, 0xFF, 4);
	}
	CC2500_Strobe(SIDLE);
	
	while(CC2500_Strobe(SNOP) != 0);
	
	CC2500_Strobe(SFRX); 
		
		// Return to RX mode
	CC2500_Strobe(SRX); 
		
		// Wait until mode changes
	while(CC2500_Strobe(SNOP) != 1);
}

void CC2500_TXData(uint8_t* data){
	
}
