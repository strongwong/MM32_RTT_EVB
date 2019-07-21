////////////////////////////////////////////////////////////////////////////////
#define _I2C_C_
////////////////////////////////////////////////////////////////////////////////

#include <string.h>

#include "EVBoard.h"


////////////////////////////////////////////////////////////////////////////////
void initGPIO_I2C(I2C_TypeDef *I2Cx)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	if(I2Cx==I2C1){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);  	

		GPIO_SetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);	
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
		
		GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8 | GPIO_Pin_9;
	}
	if(I2Cx==I2C2){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE); 
		
		GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11);	
		
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10 | GPIO_Pin_11; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
		
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	}
}


////////////////////////////////////////////////////////////////////////////////
void NVIC_I2C(I2C_TypeDef* I2Cx)
{
  NVIC_InitTypeDef  NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
  if(I2Cx == I2C1)
  	NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
  
  if(I2Cx == I2C2)
  	NVIC_InitStructure.NVIC_IRQChannel = I2C2_EV_IRQn;
  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

////////////////////////////////////////////////////////////////////////////////
void i2cInitMaster(I2C_TypeDef *I2Cx, u8 sla, u32 uiI2C_speed) 
{
	I2C_InitTypeDef I2C_InitStructure;
	
	I2C_InitStructure.I2C_Mode = I2C_Mode_MASTER;
	I2C_InitStructure.I2C_OwnAddress = 0;
	I2C_InitStructure.I2C_Speed = I2C_Speed_STANDARD;
	I2C_InitStructure.I2C_ClockSpeed = uiI2C_speed;
	
	I2C_Init(I2Cx, &I2C_InitStructure);

	I2C_ClearITPendingBit(I2C2, I2C_IT_RX_FULL | I2C_IT_TX_EMPTY | I2C_IT_STOP_DET | I2C_IT_START_DET | I2C_IT_ACTIVITY);
	
	I2C_Cmd(I2C2, DISABLE);
	I2C_Send7bitAddress(I2C2, SLAVE_ADDR , WR);
	I2C_Cmd(I2C2, ENABLE);
}


////////////////////////////////////////////////////////////////////////////////
void i2cComplete(u16 sta)
{
	I2C_ClearFlag(I2C2,sta);
	I2C_ClearITPendingBit(I2C2, I2C_IT_TX_EMPTY);
	
	i2c.rev = WR;
	i2c.sadd = TRUE;
	i2c.ack = TRUE;
	i2c.busy = FALSE;
}

////////////////////////////////////////////////////////////////////////////////
void I2C2_EV_IRQHandler()
{
	static  u8 n = 0;		// Rcv length or Send length
	static  u8 m = 0;		// Rcv length
	static	u8 _cnt;
	
	u16 status = I2C2->IC_RAW_INTR_STAT;

	if (!i2c.busy) {
		i2cComplete(status);
	}
	else if (i2c.rev == RD) {													// Read
		if (status & I2C_IT_TX_ABRT) {
			i2cComplete(status);
			i2c.ack = FALSE;
		}
		else {
			if (status & I2C_IT_TX_EMPTY) {
				(m++ < (i2c.cnt - 1)) ? I2C_ReadCmd(I2C2) : I2C_ClearITPendingBit(I2C2, I2C_IT_TX_EMPTY);
			}
			
			if (status & I2C_IT_RX_FULL) {
				*i2c.ptr++ = I2C_ReceiveData(I2C2);
				
				if  ((n++ >= (i2c.cnt - 1))){
					error.i2c = FALSE;
					rf.i2c = TRUE;
					i2cComplete(status);
				}
			}
		}
	}
	else if (status & I2C_IT_TX_EMPTY){
		if (i2c.sadd) {															// send slave address
			I2C_SendData(I2C2, i2c.sub);								
			i2c.sadd = FALSE;
			 m = 0; n = 0;
			_cnt = (i2c.opt == RD) ?  0 : i2c.cnt;								// cnt = 0 if RD , eles _cnt = i2c.cnt
		}
		else if (n < _cnt) {													// write
			I2C_SendData(I2C2, *i2c.ptr++);	
			n++;
		}
		else if  (status &  I2C_IT_STOP_DET) {
			if (i2c.opt == WR) {
				i2cComplete(status);
			}
			else {
				i2c.rev = i2c.opt;
				I2C_ReadCmd(I2C2);
				I2C_ITConfig(I2C2, I2C_IT_RX_FULL | I2C_IT_RX_OVER , ENABLE);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void I2C2_ER_IRQHandler()
{
    if(I2C_GetITStatus(I2C2, I2C_IT_STOP_DET)){
        I2C_ClearITPendingBit(I2C2, I2C_IT_STOP_DET);
	}
}

////////////////////////////////////////////////////////////////////////////////
void i2cAccess(u8 sub, u8* ptr, u16 cnt)
{
	i2c.sub = sub;
	i2c.cnt = cnt;
	i2c.ptr = ptr;

	i2c.busy = TRUE;
	i2c.ack = FALSE;
	i2c.error = FALSE;
	i2c.sadd = TRUE;
	i2c.rev = WR;
	I2C_ITConfig(I2C2, I2C_IT_TX_EMPTY | I2C_IT_STOP_DET | I2C_IT_START_DET | I2C_IT_ACTIVITY, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
void i2cSendPacket(u8 sub, u8* ptr, u16 cnt)
{
	i2c.opt = WR;
	i2cAccess(sub,  ptr,  cnt);
}
	 
////////////////////////////////////////////////////////////////////////////////
void i2cRcvPacket(u8 sub, u8* ptr, u16 cnt )
{
	i2c.opt = RD;
	i2cAccess(sub,  ptr,  cnt);
}

////////////////////////////////////////////////////////////////////////////////
void i2cRead(u8 sub, u8* ptr, u16 len)
{
	do{	
		i2cRcvPacket(sub, ptr, len);	
		while(i2c.busy);
	} 
	while(!i2c.ack);
}

////////////////////////////////////////////////////////////////////////////////
void i2cWrite(u8 sub, u8* ptr, u16 len)
{
	do{	
		i2cSendPacket(sub, ptr, len);	
		while(i2c.busy);
	} 
	while(!i2c.ack);
}

////////////////////////////////////////////////////////////////////////////////
void i2cCheck()
{
	i2cRcvPacket(0, &i2cRx[0], 8);	
	while(!i2c.ack);
	
	if (memcmp(i2cRx, i2cTx, 8) != 0) {
		i2cSendPacket(0, &i2cTx[0], 8);
		while(i2c.busy);
	}
}

////////////////////////////////////////////////////////////////////////////////
void initI2C(void)
{
	i2cInitMaster(I2C2, SLAVE_ADDR, 100000);
	i2cCheck();
}

////////////////////////////////////////////////////////////////////////////////
