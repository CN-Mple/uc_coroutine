#include "main.h"

#include <stdio.h>

#include "SEGGER_RTT.h"
#include "uc_coroutine.h"

int fputc(int ch, FILE *f)
{
        (void)f;
#if 0
        LL_USART_TransmitData8(USART1, (uint8_t)ch);
	while (!LL_USART_IsActiveFlag_TC(USART1));
	LL_USART_ClearFlag_TC(USART1);
#else
        SEGGER_RTT_Write(0, (uint8_t*)&ch, 1);
#endif
	return (ch);
}


static void SystemClock_Config(void);
static void default_init(void);

static void default_init(void)
{
        LL_USART_InitTypeDef USART_InitStruct;
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
        LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1);
        
        LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_ALTERNATE);
        LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_2, LL_GPIO_AF1_USART1);
        LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_3, LL_GPIO_MODE_ALTERNATE);
        LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_3, LL_GPIO_AF1_USART1);
        
        USART_InitStruct.BaudRate = 115200;
        USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
        USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
        USART_InitStruct.Parity = LL_USART_PARITY_NONE;
        USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
        USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
        USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
        
        LL_USART_Init(USART1, &USART_InitStruct);
        LL_USART_ConfigAsyncMode(USART1);
        LL_USART_Enable(USART1);

        NVIC_SetPriority(USART1_IRQn, 0);
        NVIC_EnableIRQ(USART1_IRQn);
}

void fib(void *arg)
{
        uint32_t max = (uint32_t)arg;
        uint32_t a = 0;
        uint32_t b = 1;

        while (a < max) {
                generator_yield((void*)a);
                uint32_t c = a + b;
                a = b;
                b = c;
        }
}

int main(void)
{
        LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
        SystemClock_Config();

        SEGGER_RTT_Init();
        
        default_init();
        LL_mDelay(1);
        printf(" \r\n");
        printf("**************************************\n");
        printf("*          HELLO-AIR001!             *\n");
        printf("*      WELCOME TO MCU WORLD!         *\n");
        printf("*                                    *\n");
        printf("**************************************\n");
        printf(" \r\n");
#if 1
        generator_init();
#if 0
        struct Generator *g = generator_create(fib);
        foreach(value, g, (void*)(1000 * 1000))
                printf("[%d]\r\n", (int)value);
        generator_destory(g);
        (void)g;
#else
        struct Generator *ga = generator_create(fib);
        void *ga_value = 0;
        do_next(ga_value, ga, (void*)10);
        
        struct Generator *gb = generator_create(fib);
        void *gb_value = 0;
        do_next(gb_value, gb, (void*)1000);
        
        while(true) {
                do_next(ga_value, ga, 0);
                if(!ga->dead)
                        printf("[ga][%d]\r\n", (int)ga_value);
                
                do_next(gb_value, gb, 0);
                if(!gb->dead)
                        printf("[gb][%d]\r\n", (int)gb_value);
                
                if(ga->dead && gb->dead) break;
        }
        
        generator_destory(ga);
        generator_destory(gb);
        (void)ga;
        (void)gb;
#endif
#endif
        printf("Enter main while!\r\n");
        
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
        LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_0, LL_GPIO_MODE_OUTPUT);
        while (1)
        {
                LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_0);
                LL_mDelay(1000);
        }
}

static void SystemClock_Config(void)
{
        uint32_t clock_source_frequency = 0;

        LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

        LL_RCC_HSI_Enable();
#if HSI_24MHZ
        LL_RCC_HSI_SetCalibFreq(LL_RCC_HSICALIBRATION_16MHz);
        clock_source_frequency = 24000000;
#elif HSI_16MHZ
        LL_RCC_HSI_SetCalibFreq(LL_RCC_HSICALIBRATION_16MHz);
        clock_source_frequency = 16000000;
#elif HSI_8MHZ
        LL_RCC_HSI_SetCalibFreq(LL_RCC_HSICALIBRATION_8MHz);
        clock_source_frequency =  8000000;
#endif
        while (LL_RCC_HSI_IsReady() != 1)
                ;

        LL_RCC_PLL_SetMainSource(LL_RCC_PLLSOURCE_HSI);
        LL_RCC_PLL_Enable();
        while (LL_RCC_PLL_IsReady() != 1)
                ;

        LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
        LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
#if 1
        LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
        while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

        LL_Init1msTick(2 * clock_source_frequency);
        LL_SetSystemCoreClock(2 * clock_source_frequency);
#else
        LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSISYS);
        while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSISYS);

        LL_Init1msTick(clock_source_frequency);
        LL_SetSystemCoreClock(clock_source_frequency);
#endif
}

void Error_Handler(void)
{
    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
    while (1)
    {
    }
}
#endif
