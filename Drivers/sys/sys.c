#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//ϵͳʱ�ӳ�ʼ��	
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/6/8
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵��
//��
////////////////////////////////////////////////////////////////////////////////// 

//ʹ��CPU��L1-Cache
void Cache_Enable(void)
{
    SCB_EnableICache();//ʹ��I-Cache
    SCB_EnableDCache();//ʹ��D-Cache   
	SCB->CACR|=1<<2;   //ǿ��D-Cache͸д,�粻����,ʵ��ʹ���п���������������	
}

//ʱ�����ú���
//Fvco=Fs*(plln/pllm);
//Fsys=Fvco/pllp=Fs*(plln/(pllm*pllp));
//Fq=Fvco/pllq=Fs*(plln/(pllm*pllq));

//Fvco:VCOƵ��
//Fsys:ϵͳʱ��Ƶ��,Ҳ��PLL1��p��Ƶ���ʱ��Ƶ��
//Fq:PLL1��q��Ƶ���ʱ��Ƶ��
//Fs:PLL����ʱ��Ƶ��,������HSI,CSI,HSE��. 

//plln:PLL1��Ƶϵ��(PLL��Ƶ),ȡֵ��Χ:4~512.
//pllm:PLL1Ԥ��Ƶϵ��(��PLL֮ǰ�ķ�Ƶ),ȡֵ��Χ:2~63.
//pllp:PLL1��p��Ƶϵ��(PLL֮��ķ�Ƶ),��Ƶ����Ϊϵͳʱ��,ȡֵ��Χ:2~128.(�ұ�����2�ı���)
//pllq:PLL1��q��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:1~128.

//CPUƵ��(rcc_c_ck)=sys_d1cpre_ck=400Mhz 
//rcc_aclk=rcc_hclk3=200Mhz
//AHB1/2/3/4(rcc_hclk1/2/3/4)=200Mhz  
//APB1/2/3/4(rcc_pclk1/2/3/4)=100Mhz  
//FMCʱ��Ƶ��=pll2_r_ck=((25/25)*512/2)=256Mhz

//�ⲿ����Ϊ25M��ʱ��,�Ƽ�ֵ:plln=160,pllm=5,pllp=2,pllq=2.
//�õ�:Fvco=25*(160/5)=800Mhz
//     Fsys=800/2=400Mhz
//     Fq=800/2=400Mhz
//����ֵ:0,�ɹ�;1,ʧ�ܡ�
u8 Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq)
{
	HAL_StatusTypeDef ret=HAL_OK;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;
  
	MODIFY_REG(PWR->CR3,PWR_CR3_SCUEN, 0);
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	while ((PWR->D3CR & (PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY) {}
  
	RCC_OscInitStruct.OscillatorType=RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState=RCC_HSE_ON;
	RCC_OscInitStruct.HSIState=RCC_HSI_OFF;
	RCC_OscInitStruct.CSIState=RCC_CSI_OFF;
	RCC_OscInitStruct.PLL.PLLState=RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource=RCC_PLLSOURCE_HSE;

	RCC_OscInitStruct.PLL.PLLN=plln;
	RCC_OscInitStruct.PLL.PLLM=pllm;
	RCC_OscInitStruct.PLL.PLLP=pllp;
	RCC_OscInitStruct.PLL.PLLQ=pllq;

	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
	ret=HAL_RCC_OscConfig(&RCC_OscInitStruct);
	if(ret!=HAL_OK) return 1;
		
	QSPI_Enable_Memmapmode();				//QSPI�ڴ�ӳ��ģʽ,��Ҫ��ʱ�ӳ�ʼ��֮ǰ����,������и�������
  
	RCC_ClkInitStruct.ClockType=(RCC_CLOCKTYPE_SYSCLK|\
                                RCC_CLOCKTYPE_HCLK |\
                                RCC_CLOCKTYPE_D1PCLK1 |\
                                RCC_CLOCKTYPE_PCLK1 |\
                                RCC_CLOCKTYPE_PCLK2 |\
                                RCC_CLOCKTYPE_D3PCLK1);

	RCC_ClkInitStruct.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.SYSCLKDivider=RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider=RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider=RCC_APB1_DIV2; 
	RCC_ClkInitStruct.APB2CLKDivider=RCC_APB2_DIV2; 
	RCC_ClkInitStruct.APB3CLKDivider=RCC_APB3_DIV2;  
	RCC_ClkInitStruct.APB4CLKDivider=RCC_APB4_DIV4; 
	ret=HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
	if(ret!=HAL_OK) return 1;

	__HAL_RCC_CSI_ENABLE() ;
	__HAL_RCC_SYSCFG_CLK_ENABLE() ;  
	HAL_EnableCompensationCell();
	
	return 0;
}


#ifdef  USE_FULL_ASSERT
//��������ʾ�����ʱ��˺����������������ļ���������
//file��ָ��Դ�ļ�
//line��ָ�����ļ��е�����
void assert_failed(uint8_t* file, uint32_t line)
{ 
	while (1)
	{
	}
}
#endif

//�ж�I_Cache�Ƿ��
//����ֵ:0 �رգ�1 ��
u8 Get_ICahceSta(void)
{
    u8 sta;
    sta=((SCB->CCR)>>17)&0X01;
    return sta;
}

//�ж�I_Dache�Ƿ��
//����ֵ:0 �رգ�1 ��
u8 Get_DCahceSta(void)
{
    u8 sta;
    sta=((SCB->CCR)>>16)&0X01;
    return sta;
}

//QSPI�����ڴ�ӳ��ģʽ��ִ��QSPI����ر�ǰ�ᣬΪ�˼���������ļ���
//����GPIO�����⣬�������������������üĴ�����ʽ��
void QSPI_Enable_Memmapmode(void)
{
	u32 tempreg=0; 
	vu32 *data_reg=&QUADSPI->DR;
	GPIO_InitTypeDef qspi_gpio;
	
	RCC->AHB4ENR|=1<<1;    						//ʹ��PORTBʱ��	   
	RCC->AHB4ENR|=1<<5;    						//ʹ��PORTFʱ��	   
	RCC->AHB3ENR|=1<<14;   						//QSPIʱ��ʹ��

	qspi_gpio.Pin=GPIO_PIN_6;					//PB6 AF10	
	qspi_gpio.Mode=GPIO_MODE_AF_PP;
	qspi_gpio.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
	qspi_gpio.Pull=GPIO_NOPULL;
	qspi_gpio.Alternate=GPIO_AF10_QUADSPI;
	HAL_GPIO_Init(GPIOB,&qspi_gpio);
	
	qspi_gpio.Pin=GPIO_PIN_2;					//PB2 AF9	
	qspi_gpio.Alternate=GPIO_AF9_QUADSPI;
	HAL_GPIO_Init(GPIOB,&qspi_gpio);
	
	qspi_gpio.Pin=GPIO_PIN_6|GPIO_PIN_7;		//PF6,7 AF9	
	qspi_gpio.Alternate=GPIO_AF9_QUADSPI;
	HAL_GPIO_Init(GPIOF,&qspi_gpio);
	
	qspi_gpio.Pin=GPIO_PIN_8|GPIO_PIN_9;		//PF8,9 AF10		
	qspi_gpio.Alternate=GPIO_AF10_QUADSPI;
	HAL_GPIO_Init(GPIOF,&qspi_gpio);
	
	//QSPI���ã��ο�QSPIʵ���QSPI_Init����
	RCC->AHB3RSTR|=1<<14;			//��λQSPI
	RCC->AHB3RSTR&=~(1<<14);		//ֹͣ��λQSPI
	while(QUADSPI->SR&(1<<5));		//�ȴ�BUSYλ���� 
	QUADSPI->CR=0X01000310;			//����CR�Ĵ���,��Щֵ��ô���ģ���ο�QSPIʵ��/��H750�ο��ֲ�Ĵ�����������
	QUADSPI->DCR=0X00160401;		//����DCR�Ĵ���
	QUADSPI->CR|=1<<0;				//ʹ��QSPI 

	//ע��:QSPI QEλ��ʹ�ܣ���QSPI��д�㷨���棬���Ѿ�������
	//����,������Բ�������QEλ��������Ҫ�����QEλ��1�Ĵ���
	//����,�������ͨ������������,ֱ����¼���ⲿQSPI FLASH,�ǲ����õ�
	//�����ֱ����¼���ⲿQSPI FLASHҲ������,����Ҫ���������QEλ��1�Ĵ���
	
	//W25QXX����QPIģʽ��0X38ָ�
	while(QUADSPI->SR&(1<<5));		//�ȴ�BUSYλ���� 
	QUADSPI->CCR=0X00000138;		//����0X38ָ�W25QXX����QPIģʽ
	while((QUADSPI->SR&(1<<1))==0);	//�ȴ�ָ������
	QUADSPI->FCR|=1<<1;				//���������ɱ�־λ 	

	//W25QXXдʹ�ܣ�0X06ָ�
	while(QUADSPI->SR&(1<<5));		//�ȴ�BUSYλ���� 
	QUADSPI->CCR=0X00000106;		//����0X06ָ�W25QXXдʹ��
	while((QUADSPI->SR&(1<<1))==0);	//�ȴ�ָ������
	QUADSPI->FCR|=1<<1;				//���������ɱ�־λ 
	
	//W25QXX����QPI��ض�������0XC0��
	while(QUADSPI->SR&(1<<5));		//�ȴ�BUSYλ���� 
	QUADSPI->CCR=0X030003C0;		//����0XC0ָ�W25QXX����������
	QUADSPI->DLR=0;
	while((QUADSPI->SR&(1<<2))==0);	//�ȴ�FTF
	*(vu8 *)data_reg=3<<4;			//����P4&P5=11,8��dummy clocks,104M
	QUADSPI->CR|=1<<2;				//��ֹ���� 
	while((QUADSPI->SR&(1<<1))==0);	//�ȴ����ݷ������
	QUADSPI->FCR|=1<<1;				//���������ɱ�־λ  
	while(QUADSPI->SR&(1<<5));		//�ȴ�BUSYλ���� 	 

	//MemroyMap ģʽ����
	while(QUADSPI->SR&(1<<5));		//�ȴ�BUSYλ���� 
	QUADSPI->ABR=0;					//�����ֽ�����Ϊ0��ʵ���Ͼ���W25Q 0XEBָ���,M0~M7=0
	tempreg=0XEB;					//INSTRUCTION[7:0]=0XEB,����0XEBָ�Fast Read QUAD I/O��
	tempreg|=3<<8;					//IMODE[1:0]=3,���ߴ���ָ��
	tempreg|=3<<10;					//ADDRESS[1:0]=3,���ߴ����ַ
	tempreg|=2<<12;					//ADSIZE[1:0]=2,24λ��ַ����
	tempreg|=3<<14;					//ABMODE[1:0]=3,���ߴ��佻���ֽ�
	tempreg|=0<<16;					//ABSIZE[1:0]=0,8λ�����ֽ�(M0~M7)
	tempreg|=6<<18;					//DCYC[4:0]=6,6��dummy����
	tempreg|=3<<24;					//DMODE[1:0]=3,���ߴ�������
	tempreg|=3<<26;					//FMODE[1:0]=3,�ڴ�ӳ��ģʽ
	QUADSPI->CCR=tempreg;			//����CCR�Ĵ���
	
	//����QSPI FLASH�ռ��MPU����
	SCB->SHCSR&=~(1<<16);			//��ֹMemManage 
	MPU->CTRL&=~(1<<0);				//��ֹMPU
	MPU->RNR=0;						//���ñ���������Ϊ0(1~7���Ը������ڴ���)
	MPU->RBAR=0X90000000;			//����ַΪ0X9000 000,��QSPI����ʼ��ַ
	MPU->RASR=0X0303002D;			//������ر�������(��ֹ����,����cache,������),���MPUʵ��Ľ���
	MPU->CTRL=(1<<2)|(1<<0);		//ʹ��PRIVDEFENA,ʹ��MPU 
	SCB->SHCSR|=1<<16;				//ʹ��MemManage
}

#if defined(__clang__) //ʹ��V6������(clang)
//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
void __attribute__((noinline)) WFI_SET(void)
{
    __asm__("wfi");
}

//�ر������ж�(���ǲ�����fault��NMI�ж�)   
void __attribute__((noinline)) INTX_DISABLE(void)
{
    __asm__("cpsid i \t\n"
            "bx lr");
}

//���������ж�
void __attribute__((noinline)) INTX_ENABLE(void)
{
    __asm__("cpsie i \t\n"
            "bx lr");
}

//����ջ����ַ
//addr:ջ����ַ
void __attribute__((noinline)) MSR_MSP(u32 addr) 
{
    __asm__("msr msp, r0 \t\n"
            "bx r14");
}
#elif defined (__CC_ARM)    //ʹ��V5������(ARMCC)

//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
__asm void WFI_SET(void)
{
	WFI;		  
}
//�ر������ж�(���ǲ�����fault��NMI�ж�)
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
//���������ж�
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}
#endif
