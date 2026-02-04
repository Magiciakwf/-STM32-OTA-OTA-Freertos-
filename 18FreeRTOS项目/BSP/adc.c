#include "adc.h"


uint16_t DMA_Buf[Buf_Size];

void UserADC_Init(void)
{
	ADC_InitTypeDef ADC_StructureInit;
	DMA_InitTypeDef DMA_StructureInit;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	ADC_TempSensorVrefintCmd(ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//12MHZ
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 2, ADC_SampleTime_239Cycles5);
	
	ADC_StructureInit.ADC_ContinuousConvMode = ENABLE;
	ADC_StructureInit.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_StructureInit.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_StructureInit.ADC_Mode = ADC_Mode_Independent;
	ADC_StructureInit.ADC_NbrOfChannel = 2;//开启的通道数
	ADC_StructureInit.ADC_ScanConvMode = ENABLE;
	
	DMA_StructureInit.DMA_BufferSize = Buf_Size;
	DMA_StructureInit.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_StructureInit.DMA_M2M = DMA_M2M_Disable;
	DMA_StructureInit.DMA_MemoryBaseAddr = (uint32_t)DMA_Buf;
	DMA_StructureInit.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//配半个字，ADC数据为12位
	DMA_StructureInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_StructureInit.DMA_Mode = DMA_Mode_Circular;
	DMA_StructureInit.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	DMA_StructureInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;////配半个字，ADC数据为12位
	DMA_StructureInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_StructureInit.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA1_Channel1, &DMA_StructureInit);
	
	ADC_Init(ADC1, &ADC_StructureInit);
	ADC_DMACmd(ADC1, ENABLE);//使能ADC_DMA
	ADC_Cmd(ADC1, ENABLE);//使能ADC
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1) == SET);
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1) == SET);
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//软件触发
}