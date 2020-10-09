/******************************************************************
 * 文件：SPIx.c
 * 功能：定义SPI功能函数
 * 日期：2018-03-17
 * 作者：zx
 * 版本：Ver.1.0 | 最初版本
 * 
 * Copyright (C) 2018 zx. All rights reserved.
*******************************************************************/
#include "SPIx/SPIx.h"

/**
 * 功能：初始化SPI相关引脚和SPI外设工作状态
 * 参数：
 *       SPIx：指定待初始化的SPI，SPI1-SPI3
 * 返回值：None
 */
void initSPIx(SPI_TypeDef* SPIx)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    if(SPIx==SPI1)
    {
        /*按需添加*/
    }else if(SPIx==SPI2)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);                //使能SPI2时钟       
    }else if(SPIx==SPI3)
    {
        /*按需添加*/
    }else 
    {
       /*按需添加*/ 
    }
    /***********************SPI GPIO配置****************************/
    if(SPIx==SPI1)
    {
        /*按需添加*/
    }else if(SPIx==SPI2)
    {
        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;                      //配置PA8为NSS选通引脚
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                    
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        GPIO_SetBits(GPIOA,GPIO_Pin_8);                                 //默认拉高

        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13|GPIO_Pin_15;         //配置MOSI(PB15)和SCK(PB13)
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                                 
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14;                     //配置MISO(PB14)
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;                                 
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }else if(SPIx==SPI3)
    {
        /*按需添加*/
    }else 
    {
       /*按需添加*/ 
    }
    /***********************SPI 工作模式****************************/
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置两线全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                       //设置为SPI主机
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                   //8位数据帧格式
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                          //时钟线空闲低电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                        //第一个时钟沿(上升沿)采样，和SX1278一致
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                           //软件控制NSS选通
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//没有速度要求尽量设置的慢一点，这样可以兼容更多设备
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                  //高位在前传输，和SX1278一致
    SPI_InitStructure.SPI_CRCPolynomial = 7;                            //设置默认值即可
    SPI_Init(SPIx, &SPI_InitStructure);                                 //配置生效

    SPI_Cmd(SPIx, ENABLE);                                              //开启SPI
}

/**
 * 功能：设置NSS引脚状态
 * 参数：
 *       status:非0值表示拉高NSS,0表示拉低NSS
 * 返回值：None
 */
void setNSS(u8 status)
{
    if(status != 0) //拉高NSS
    {
        GPIO_SetBits(GPIOA,GPIO_Pin_8);  
    }else          //拉低NSS
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_8);  
    }
}

/**
 * 功能：发送一字节
 * 参数：
 *       SPIx:指定SPI
 *       byte:待发送数据
 * 返回值：None
 */
void sendSPIxByte(SPI_TypeDef* SPIx, u16 byte)
{
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);     //等待发送缓冲区为空
    SPI_I2S_ClearFlag(SPIx,SPI_I2S_FLAG_TXE);

    SPI_I2S_SendData(SPIx, byte);

    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);     //等待接收缓冲区非空
    SPI_I2S_ClearFlag(SPIx,SPI_I2S_FLAG_RXNE);
	
	SPI_I2S_ReceiveData(SPIx);
}
/**
 * 功能：接收一字节
 * 参数：
 *       SPIx:指定SPI
 * 返回值：读取到的数据
 */
u16 readSPIxByte(SPI_TypeDef* SPIx)
{
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);     //等待发送缓冲区为空
    SPI_I2S_ClearFlag(SPIx,SPI_I2S_FLAG_TXE);

    SPI_I2S_SendData(SPIx, 0x31);

    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);     //等待接收缓冲区非空
    SPI_I2S_ClearFlag(SPIx,SPI_I2S_FLAG_RXNE);
	
	return SPI_I2S_ReceiveData(SPIx);
}
