/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   FreeRTOS v9.0.0 + STM32 ��̬��������
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32ȫϵ�п����� 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
/*
freertos+lvgl��ֲʧ�ܣ�ԭ��ѡ���f103ze��flash 512 ram 64k,ʵ����Ҫ  ��freertos��40k+��lvgl��32k=72k  64<72  �������пռ䲻��
ѡ������ramоƬ  80��96��128��256��512   ����ʹ������ram
*/ 
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
/* ������Ӳ��bspͷ�ļ� */
#include "bsp_led.h"
#include "bsp_usart.h"
#include "tftlcd.h" 
#include "touch.h"
#include "SysTick.h"
#include "system.h"
#include "time.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_sw_test.h"
#include "sram.h"
/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
 /* ���������� */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* LED������ */
static TaskHandle_t LED_Task_Handle = NULL;
/*��������*/
/* LED������ */
static TaskHandle_t TOUCH_Task_Handle = NULL;
/********************************** �ں˶����� *********************************/
/*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */


/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */


/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void AppTaskCreate(void);/* ���ڴ������� */

static void LED_Task(void* pvParameters);/* LED_Task����ʵ�� */

static void BSP_Init(void);/* ���ڳ�ʼ�����������Դ */

static void TOUCH_Task(void* pvParameters);/* LED_Task����ʵ�� */
/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */

  /* ������Ӳ����ʼ�� */
  BSP_Init();
  printf("����һ��[Ұ��]-STM32ȫϵ�п�����-FreeRTOS-��̬��������!\r\n");
   /* ����AppTaskCreate���� */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
                        (const char*    )"AppTaskCreate",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )1, /* ��������ȼ� */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
  /* ����������� */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* �������񣬿������� */
  else
    return -1;  
  
  while(1);   /* ��������ִ�е����� */    
}


/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  taskENTER_CRITICAL();           //�����ٽ���
  
  /* ����LED_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )LED_Task, /* ������ں��� */
                        (const char*    )"LED_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )2,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&LED_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    printf("����LED_Task����ɹ�!\r\n");
    /* ����LED_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )TOUCH_Task, /* ������ں��� */
                        (const char*    )"TOUCH_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )2,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&TOUCH_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    printf("����TOUCH_Task����ɹ�!\r\n");
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}



/**********************************************************************
  * @ ������  �� LED_Task
  * @ ����˵���� LED_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void LED_Task(void* parameter)
{	
    while (1)
    {
        LED1_ON;
        vTaskDelay(500);   /* ��ʱ500��tick */
        printf("LED_Task Running,LED1_ON\r\n");
        
        LED1_OFF;     
        vTaskDelay(500);   /* ��ʱ500��tick */		 		
        printf("LED_Task Running,LED1_OFF\r\n");
    }
}

void display_init()  //��ʼ����ʾ
{
	FRONT_COLOR=RED;
	LCD_ShowString(tftlcd_data.width-8*4,0,tftlcd_data.width,tftlcd_data.height,16,"RST");
	LCD_Fill(120, tftlcd_data.height - 16, 139, tftlcd_data.height, BLUE);
    LCD_Fill(140, tftlcd_data.height - 16, 159, tftlcd_data.height, RED);
    LCD_Fill(160, tftlcd_data.height - 16, 179, tftlcd_data.height, MAGENTA);
    LCD_Fill(180, tftlcd_data.height - 16, 199, tftlcd_data.height, GREEN);
    LCD_Fill(200, tftlcd_data.height - 16, 219, tftlcd_data.height, CYAN);
    LCD_Fill(220, tftlcd_data.height - 16, 239, tftlcd_data.height, YELLOW);
}

/**********************************************************************
  * @ ������  �� TOUCH_Task
  * @ ����˵���� LED_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void TOUCH_Task(void* parameter)
{	
	 u16 penColor = BLUE;
    while (1)
    {
//		if(TP_Scan(0))
//        {   
//			/* ѡ�񻭱ʵ���ɫ */
//            if(tp_dev.y[0] > tftlcd_data.height - 18&&tp_dev.y[0]<tftlcd_data.height)
//            {
//                if(tp_dev.x[0]>220)
//                {
//                    penColor = YELLOW;
//                }
//                else if(tp_dev.x[0]>200)
//                {
//                    penColor = CYAN;
//                }
//                
//                else if(tp_dev.x[0]>180)
//                {
//                    penColor = GREEN;
//                }
//                else if(tp_dev.x[0]>160)
//                {
//                   penColor = MAGENTA;
//                }
//                else if(tp_dev.x[0]>140)
//                {
//                    penColor = RED;

//                }
//                else if(tp_dev.x[0]>120)
//                {
//                    penColor = BLUE;
//                }       
//            }
//            else   //����
//            {
//                LCD_Fill(tp_dev.x[0]-1, tp_dev.y[0]-1, tp_dev.x[0]+2,
//				tp_dev.y[0]+2, penColor);
//            }
//    		
//            /* ���� */
//            if ((tp_dev.x[0] > tftlcd_data.width-8*4) && (tp_dev.y[0] < 16))
//		   	{
//				        LCD_Fill(0, 0, tftlcd_data.width-1,tftlcd_data.height-16-1, BACK_COLOR);
//                LCD_ShowString(tftlcd_data.width-8*4,0,tftlcd_data.width,tftlcd_data.height,16,"RST");
//			   }            
//        }
//		delay_ms(10);
		tp_dev.scan(0);
		lv_task_handler();
    }
}

/***********************************************************************
  * @ ������  �� BSP_Init
  * @ ����˵���� �弶�����ʼ�������а����ϵĳ�ʼ�����ɷ��������������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  *********************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
	 * ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
	 * ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED ��ʼ�� */
	LED_GPIO_Config();

	/* ���ڳ�ʼ��	*/
	USART_Config();
  TP_Init();//����ģ���ʼ��
  TFTLCD_Init();	//��ʼ����ʾģ��
	FSMC_SRAM_Init();//��ʼ���ⲿ��չsramģ��  1M
	/*FRONT_COLOR=RED;
	LCD_ShowString(10,10,tftlcd_data.width,tftlcd_data.height,16,"Welcom");
	LCD_Clear(WHITE);
	delay_ms(1000);
	display_init();*/
	TIM4_Init(999,71);//Ϊlvgl�ṩ����
	lv_init();	//lvglϵͳ��ʼ��
	lv_port_disp_init();	//lvgl��ʾ�ӿڳ�ʼ��,����lv_init()�ĺ���
	lv_port_indev_init();	//lvgl����ӿڳ�ʼ��,����lv_init()�ĺ���
	
	lv_sw_test_start();	//��������
}

/********************************END OF FILE****************************/
