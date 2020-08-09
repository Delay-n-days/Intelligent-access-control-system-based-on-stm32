#include "stm32f10x.h"
#include "renti.h"
#include "delay.h"
void RE_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;    //LED0-->PA.4
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //上拉输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);        //初始化
}

u8 RE_Scan(u8 mode)
{
    static u8 key_up = 1;
    if (mode)
        key_up = 1;

    if (key_up && RE0 == 1)
    {
        delay_ms(5);
        key_up = 0;
        if (RE0 == 1)
        {
            return 1;
        }
    }
    else if (RE0 == 0)
    {
        key_up = 1;
    }
    return 0;
}
