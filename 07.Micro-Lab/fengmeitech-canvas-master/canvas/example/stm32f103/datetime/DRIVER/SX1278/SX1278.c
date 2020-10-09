/******************************************************************
 * 文件：SX1278.c
 * 功能：定义SPI功能函数
 * 日期：2018-03-17
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 声明：移植自Semtech和凌承芯官方Demo
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#include "SX1278/SX1278.h"
#include "SPIx/SPIx.h"
#include "DELAY/Delay.h"

unsigned char   Frequency[3]={0x6c,0x80,0x00};

unsigned char   SpreadingFactor=7;             //扩频因子,7-12,扩频因子选小一些，发射时间会更快一些。

unsigned char   CodingRate=2;                   //编码率,1-4

unsigned char   Bw_Frequency=9;                 //信号带宽,6-9

unsigned char   powerValue= 15;                 //功率设置,作为power_data数组的索引
unsigned char   power_data[16]={0X80,0X81,0X82,0X83,0X84,0X85,0X86,0X87,0X88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f};

unsigned char   RF_EX0_STATUS;

unsigned char   CRC_Value;

unsigned char   SX1278_RLEN;

/***************************读写SX1278的基础函数********************************/
/**
 * 功能：向SX1278指定地址中写入数据，通常用作更改某寄存器中的值
 * 参数：
 *       addr:寄存器地址，寄存器地址参考头文件或者官方手册
 *            读或写由地址最高位决定
 *       byte:待发送数据
 * 返回值：None
 */
void writeSX1278(unsigned char addr, unsigned char buffer)
{ 
	setNSS(0);                      //拉低NSS
	sendSPIxByte(SPI2, addr|0x80);
	sendSPIxByte(SPI2, buffer);
	setNSS(1);                      //拉高NSS
}

/**
 * 功能：读取SX1278返回的数据
 * 参数：
 *       addr:寄存器地址，寄存器地址参考头文件或者官方手册
 *            读或写由地址最高位决定
 * 返回值：返回指定寄存器当中的值
 */	
unsigned char readSX1278(unsigned char addr)
{
	unsigned char Value;

	setNSS(0);                      //拉低NSS
	sendSPIxByte(SPI2, addr&0x7f);
	Value = readSPIxByte(SPI2);
	setNSS(1);                     //拉高NSS
	
	return Value; 
}
/**********************************~END~************************************/	

/******************************配置SX1278函数********************************/
/**
 * 功能：设置SX1278工作状态
 * 参数：
 *       opMode:操作模式 取值见RFMode_SET枚举
 * 返回值：None
 */	
void setSX1278OpMode( RFMode_SET opMode )
{
	unsigned char opModePrev;
	opModePrev=readSX1278(REG_LR_OPMODE);     //读0x01模式寄存器
	opModePrev &=0xf8;                        //清零低三位
	opModePrev |= (unsigned char)opMode;      //或上形参
	writeSX1278(REG_LR_OPMODE, opModePrev);   //重新写回去	
}

/**
 * 功能：设置SX1278工作模式
 * 参数：
 *       opMode:分为FSK和LoRa两种，通常选择LoRa
 * 返回值：None
 */	
void setSX1278LoRaFSK( Debugging_fsk_ook opMode )
{
	unsigned char opModePrev;
	opModePrev=readSX1278(REG_LR_OPMODE); //读0x01模式寄存器
	opModePrev &=0x7F; //清零最高位
	opModePrev |= (unsigned char)opMode;  //或上形参
	writeSX1278( REG_LR_OPMODE, opModePrev); //重新写回去		
}

/**
 * 功能：设置SX1278射频频率
 * 参数：None
 * 返回值：None
 * 补充：射频频率= (Frequency[0]<<16 | Frequency[1]<<8 |  Frequency[2])*61.035
 *       即射频频率 = 0x6C8000 * 61.035 = 433.99MHz
 */	
void setSX1278RFFrequency(void)
{
	writeSX1278( REG_LR_FRFMSB, Frequency[0]);  //写0x06寄存器
	writeSX1278( REG_LR_FRFMID, Frequency[1]);  //写0x07寄存器
	writeSX1278( REG_LR_FRFLSB, Frequency[2]);  //写0x08寄存器
}

/**
 * 功能：设置SX1278射频功率
 * 参数：
 *        power：功率值数组索引
 * 返回值：None
 */	
void setSX1278RFPower(unsigned char power )
{
	writeSX1278( REG_LR_PADAC, 0x87);                   //设置更高功率
	writeSX1278( REG_LR_PACONFIG,  power_data[power] ); //若没有这一句，频谱仪几乎测不到信号,但是可以进行正常收发。
}

/**
 * 功能：SX1278数据包配置
 * 参数：
 *        value：待设置值
 * 返回值：None
 */		
void setSX1278NbTrigPeaks(unsigned char value )
{
	unsigned char RECVER_DAT;
	RECVER_DAT = readSX1278( 0x31);  //Read RegDetectOptimize,
	RECVER_DAT = ( RECVER_DAT & 0xF8 ) | value; //process;
	writeSX1278( 0x31, RECVER_DAT );  //write back;
}
/**
 * 功能：设置SX1278扩频因子
 * 参数：
 *        factor：扩频因子 7-12
 * 返回值：None
 */	
void setSX1278SpreadingFactor(unsigned char factor )
{
	unsigned char RECVER_DAT;
	setSX1278NbTrigPeaks(3); //0x03-->SF7 to SF12
	RECVER_DAT=readSX1278( REG_LR_MODEMCONFIG2); //读0x1E寄存器  
	RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG2_SF_MASK ) | ( factor << 4 );
	writeSX1278( REG_LR_MODEMCONFIG2, RECVER_DAT ); 
}
	
/**
 * 功能：设置SX1278纠错编码
 * 参数：
 *        value：纠错编码率 1-4
 * 返回值：None
 */
void setSX1278ErrCode(unsigned char value)
{	
	unsigned char RECVER_DAT;
	RECVER_DAT=readSX1278( REG_LR_MODEMCONFIG1); //读0x1D寄存器
	RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG1_CODINGRATE_MASK ) | ( value << 1 );       //纠错编码率，位3~1。
	writeSX1278( REG_LR_MODEMCONFIG1, RECVER_DAT);
}

/**
 * 功能：设置是否开启SX1278的CRC校验
 * 参数：
 *        enable：是否使能 0或1
 * 返回值：None
 */	
void setSX1278PacketCRC(BOOL_t enable )
{	
	unsigned char RECVER_DAT;
	RECVER_DAT= readSX1278( REG_LR_MODEMCONFIG2);  //读0x1E寄存器 
	RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK ) | ( enable << 2 );
	writeSX1278( REG_LR_MODEMCONFIG2, RECVER_DAT );
}

/**
 * 功能：设置SX1278扩频带宽
 * 参数：
 *        bw：带宽值 6-9
 * 返回值：None
 */	
void setSX1278BandWidth(unsigned char bw)
{
	unsigned char RECVER_DAT;
	RECVER_DAT=readSX1278( REG_LR_MODEMCONFIG1);  //读0x1D寄存器
	RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG1_BW_MASK ) | ( bw << 4 );
	writeSX1278( REG_LR_MODEMCONFIG1, RECVER_DAT );
}

/**
 * 功能：设置是否将SX1278的报头设置为隐式模式
 * 参数：
 *        enable：使能状态,0或1
 * 返回值：None
 */		
void setSX1278ImplicitHeader(BOOL_t enable)
{
	unsigned char RECVER_DAT;
	RECVER_DAT=readSX1278( REG_LR_MODEMCONFIG1 );  //读0x1D寄存器
	RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK ) | ( enable );
	writeSX1278( REG_LR_MODEMCONFIG1, RECVER_DAT );
}

/**
 * 功能：设置SX1278接收超时时间
 * 参数：
 *        value：超时值
 * 返回值：None
 */		
void setSX1278SymbTimeout(unsigned int value )
{
	unsigned char RECVER_DAT[2];
	RECVER_DAT[0]=readSX1278( REG_LR_MODEMCONFIG2 );    //读0x1E寄存器
	RECVER_DAT[1]=readSX1278( REG_LR_SYMBTIMEOUTLSB );  //读0x1F寄存器
	RECVER_DAT[0] = ( RECVER_DAT[0] & RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ) | ( ( value >> 8 ) & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK );
	RECVER_DAT[1] = value & 0xFF;
	writeSX1278( REG_LR_MODEMCONFIG2, RECVER_DAT[0]);
	writeSX1278( REG_LR_SYMBTIMEOUTLSB, RECVER_DAT[1]);
}

/**
 * 功能：设置SX1278负载字节长度
 * 参数：
 *        value：负载字节长度，最大值256,调用时要注意
 * 返回值：None
 */		
void setSX1278PayLoadLength(unsigned char value)
{
	writeSX1278( REG_LR_PAYLOADLENGTH, value );  //写0x22寄存器，RegPayloadLength
} 
	
/**
 * 功能：是否使能SX1278低速率优化
 * 参数：
 *        enable:使能状态 0或1
 * 返回值：None
 */	
void setSX1278MobileNode(BOOL_t enable )
{	 
	unsigned char RECVER_DAT;
	RECVER_DAT=readSX1278( REG_LR_MODEMCONFIG3 );  //读0x26寄存器，低数据率优化
	RECVER_DAT = ( RECVER_DAT & RFLR_MODEMCONFIG3_MOBILE_NODE_MASK ) | ( enable << 3 );
	writeSX1278( REG_LR_MODEMCONFIG3, RECVER_DAT );
}

/**
 * 功能：设置SX1278为连续接收接收模式
 * 参数：None
 * 返回值：None
 */
void setSX1278Receive (void)
{
	setSX1278OpMode(Stdby_mode );
	writeSX1278(REG_LR_IRQFLAGSMASK,IRQN_RXD_Value);  //0x11,打开接收中断
	writeSX1278(REG_LR_HOPPERIOD,PACKET_MIAX_Value);  //0x24寄存器，设置频率跳变周期为最大
	writeSX1278( REG_LR_DIOMAPPING1,0X00);            //DIO引脚映射设置，按默认
	writeSX1278( REG_LR_DIOMAPPING2,0X00);	
	setSX1278OpMode(Receiver_mode);                   //设置为连续接收模式
}

/**
 * 功能：复位SX1278,只在初始化之前复位
 * 参数：None
 * 返回值：None
 */
void resetSX1278(void)
{
    GPIO_ResetBits(GPIOB,GPIO_Pin_12);
    Delay_ms(10);
    GPIO_SetBits(GPIOB,GPIO_Pin_12);
    Delay_ms(10);
}
/**
 * 功能：初始化SX1278
 * 参数：None
 * 返回值：None
 */	
void initSX1278(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;            //配置PB12为SX1278复位引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB,GPIO_Pin_12);    

    resetSX1278();                                          //复位设备
	setSX1278OpMode(Sleep_mode);                            //设置睡眠模式
	setSX1278LoRaFSK(LORA_mode);	                        // 设置扩频模式
	setSX1278OpMode(Stdby_mode);                            // 设置为普通模式
	writeSX1278( REG_LR_DIOMAPPING1,GPIO_VARE_1);           //写0x40寄存器,DIO引脚映射设置，设为00.
	writeSX1278( REG_LR_DIOMAPPING2,GPIO_VARE_2);           //写0x41寄存器
	setSX1278RFFrequency();                                 //频率设置
	setSX1278RFPower(powerValue);                           //功率设置
	setSX1278SpreadingFactor(SpreadingFactor);	            // 扩频因子设置
	setSX1278ErrCode(CodingRate);		                    //纠错编码率设置
	setSX1278PacketCRC(true);			                    //CRC 校验打开
	setSX1278BandWidth(Bw_Frequency);	                    //设置扩频带宽, 125khz
	setSX1278ImplicitHeader(false);		                    //同步头是显性模式
	setSX1278PayLoadLength(0xff);                           //设置有效负载长度
	setSX1278SymbTimeout(0x3FF);                            //设置接收超时时间，TimeOut = SymbTimeout * Ts。
	setSX1278MobileNode(true); 	                            //低数据率优化，当有效数据速率较低时，必须使用LowDataRateOptimize位提高LoRa链路的鲁棒性。
	writeSX1278(REG_LR_IRQFLAGS, 0xff); 
	setSX1278Receive();                                     //设置为连续接收模式
}
/**
 * 功能：发送数据包
 * 参数：
 *       buffer:数据包地址
 *       len:数据包长度
 * 返回值：None
 */		
void transmitPackets(unsigned char *buffer,unsigned char len)
{	
	unsigned char i;
	
	setSX1278OpMode( Stdby_mode );
	writeSX1278( REG_LR_HOPPERIOD, 0 );	             //不做频率跳变
	writeSX1278(REG_LR_IRQFLAGSMASK,IRQN_TXD_Value); //打开发送中断
	writeSX1278( REG_LR_PAYLOADLENGTH, len);	     //最大数据包设置
	writeSX1278( REG_LR_FIFOTXBASEADDR, 0);          //写Tx FIFO基址
	writeSX1278( REG_LR_FIFOADDRPTR, 0 );            //FIFO指针
	
	setNSS(0);                                   //拉低NSS
	sendSPIxByte(SPI2,0x80);			             //0X00 | 0X80，突发访问，写
	for(i = 0;i<len;i++)                             //将待发送数据发送到FIFO并等待发送
	{
		sendSPIxByte(SPI2,*buffer);
		buffer++;
	}
	setNSS(1);                                       //拉高NSS
	writeSX1278(REG_LR_DIOMAPPING1,0x40);            //设置0x40寄存器为0100 0000b，即设置发射完成指示映射到DIO0引脚
	writeSX1278(REG_LR_DIOMAPPING2,0x00);
	setSX1278OpMode(Transmitter_mode); 

    while((readSX1278(REG_LR_IRQFLAGS)&0x08) != 0x08)//等待发送完成
    {
        Delay_ms(20);
    }

    writeSX1278(REG_LR_IRQFLAGS, 0xff);              //清零所有标志位，所有的DIOx口都会恢复低电平
}

/**
 * 功能：接收数据包
 * 参数：
 *       buffer:数据包存放地址
 * 返回值：
 * 		 如果成功接收返回1，否则返回0
 */	
unsigned char receivePackets(unsigned char* buffer)
{
    unsigned char i;
    unsigned char len;

	setSX1278Receive();
    
	if((readSX1278(REG_LR_IRQFLAGS)&0x40)==0x40)            //接收完成
	{
		if((readSX1278(REG_LR_MODEMCONFIG2)&0x04)==0x04)    //是否CRC校验完成
		{
			writeSX1278 (REG_LR_FIFOADDRPTR,0x00);          //设置SPI接口在FIFO缓冲区中的地址指针
			len = readSX1278(REG_LR_NBRXBYTES);             //读取最后一个包的字节数
			setNSS(0);
			sendSPIxByte(SPI2,0x00);                        //0X00 | 0X00，突发访问，读。
            		
			for(i=0;i<len;++i)                              //读取接收到的数据到指定数组
			{
				buffer[i]= readSPIxByte(SPI2);		
			}
			setNSS(1);

			writeSX1278(REG_LR_IRQFLAGS, 0xff); 			//清除中断标志位
			return 1;
		}

		writeSX1278(REG_LR_IRQFLAGS, 0xff);					//清除中断标志位
		return 0; 
	} 
	return 0;       
}





