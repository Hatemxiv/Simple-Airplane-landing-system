#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <task.h>
#include <semphr.h>
//PORTF
#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
#define GPIO_PORTF_DR2R_R       (*((volatile unsigned long *)0x40025500))
#define GPIO_PORTF_DR4R_R       (*((volatile unsigned long *)0x40025504))
#define GPIO_PORTF_DR8R_R       (*((volatile unsigned long *)0x40025508))
#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define PF4                     (*((volatile unsigned long *)0x40025040))
#define PF3                     (*((volatile unsigned long *)0x40025020))
#define PF2                     (*((volatile unsigned long *)0x40025010))
#define PF1                     (*((volatile unsigned long *)0x40025008))
#define PF0                     (*((volatile unsigned long *)0x40025004))
	//PORTB

#define SYSCTL_RCGCGPIO_R   (*((volatile int *) 0x400FE608))   
#define GPIO_PORTB_DEN_R    (*((volatile int *) 0x4000551C))
#define GPIO_PORTB_DIR_R    (*((volatile int *) 0x40005400))
#define GPIO_PORTB_DATA_R   (*((volatile int *) 0x400053FC))
#define GPIO_PORTB_AMSEL_R  (*((volatile int *) 0x40005528))
#define GPIO_PORTB_AFSEL_R  (*((volatile int *) 0x40005420))
#define GPIO_PORTB_PCTL_R   (*((volatile int *) 0x4000552C))
#define GPIO_PORTB_PUR_R    (*((volatile int *) 0x40005510))
#define GPIO_PORTB_LOCK_R   (*((volatile int *) 0x40005520))
#define GPIO_PORTB_CR_R     (*((volatile int *) 0x40005524))
#define GPIO_PORTB_CLK_EN  0x02      //enable clock for PORTB


void PortF_Init(void);
void landing(void *pvParameters); //intializing task 1
void takingoff(void *pvParameters); // initializing task 2
void stable(void *pvParameters); // initializing task 3
void idle(void *pvParameters); // initializing task idle
SemaphoreHandle_t sema1 = NULL;
SemaphoreHandle_t sema2 = NULL;


TaskHandle_t xlandnig = NULL ;
TaskHandle_t xtaking = NULL ;
TaskHandle_t xstable = NULL ;
void Delay(unsigned int);
void PORTB_Init(void);

void landing (void *pvParamaters)
{
	xSemaphoreGive(sema1);
	for(;;)
	{

	if(xSemaphoreTake(sema1,portMAX_DELAY)==pdTRUE)
		
	{
				if((GPIO_PORTF_DATA_R&0x11)==0x01)
				{
							xSemaphoreGive(sema1);
							GPIO_PORTF_DATA_R=0x02;
				}
				
	}
	}

}

void takingoff (void *pvParameters)
{
	xSemaphoreGive(sema2);
	for(;;)
	{

	if(xSemaphoreTake(sema2,portMAX_DELAY)==pdTRUE)
		
	{
				if((GPIO_PORTF_DATA_R&0x11)==0x01)
				{
				xSemaphoreGive(sema1);
				}
				else if((GPIO_PORTF_DATA_R&0x11)==0x10)
				{
							xSemaphoreGive(sema2);
					
							GPIO_PORTF_DATA_R|=0x04;
				}
				
	}
	}

}

void stable (void *pvParameters)
{
	for(;;)
	{
		if(GPIO_PORTF_DATA_R&0x11==0x01)
		{
		xSemaphoreGive(sema1);
		}
		else if(GPIO_PORTF_DATA_R&0x11==0x10)
		{
				xSemaphoreGive(sema2);
	
		}
	}
}

int main(void)
{
	sema1= xSemaphoreCreateBinary();
	sema2= xSemaphoreCreateBinary();
	xTaskCreate(landing,"landing task",256,NULL, 3, &xlandnig); // 100 stack depth and priority 1
	xTaskCreate(takingoff,"taking off task",256,NULL,2,&xtaking); // higher priority
	xTaskCreate(stable,"stable task",256,NULL, 1, NULL); // 
	//xTaskCreate(idle,"idle",256,NULL, 1, NULL); // 
	
		vTaskStartScheduler(); // start scahduling
	for(;;);
/*
	unsigned char digitPattern[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};  
	volatile unsigned char i;
	PORTB_Init();
	while(1)
	{
		for(i=0;i<10;i++)
		{
			GPIO_PORTB_DATA_R = digitPattern[i];   //display digits from 0 to 9 
			Delay(1000);													// delay 1 second between each digit
		}
		
	}*/
	
	
}


void Delay(unsigned int delay)
{
	volatile unsigned int i, counter;
	counter = delay * 4000;  // 1 second (1000 msec) needs 40000000 counter so 4000000/1000 = 4000
	for(i=0;i<counter;i++);
}


void PORTB_Init(void)
{
	SYSCTL_RCGCGPIO_R |= GPIO_PORTB_CLK_EN;           //activate clock for Port B
  Delay(10);           															//Delay 10 msec to allow clock to start on PORTB  
  GPIO_PORTB_DEN_R = 0xFF;        	  							// Enable all pins of PORTB 
  GPIO_PORTB_DIR_R = 0xFF;        	  							// Make all pins of PORTB as ouptut pin
	GPIO_PORTB_PCTL_R &= ~(0xFF) ; 										// Regular GPIO of PORTB
  GPIO_PORTB_AMSEL_R &= ~(0xFF);        						// Disable analog function on all pins of PORTB
	GPIO_PORTB_AFSEL_R &= ~(0xFF);        						// Regular port function
}
void PortF_Init(void){ 
	//volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x20;     // 1) activate clock for Port F
//  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 in (switches), PF1,PF2,PF1 out (leds)
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
  GPIO_PORTF_PUR_R = 0x11;          // enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital I/O on PF4-0 
	
}


