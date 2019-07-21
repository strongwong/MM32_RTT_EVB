/*
 * File      : usart.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006-2013, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2016-08-30     Aubr.Cool       the first version
 */

#include <stm32l0xx.h>

#include <rtthread.h>
#include "usart.h"

#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_usart.h"

#ifdef RT_USING_UART

#ifdef RT_USING_DEVICE
#include <rtdevice.h>
#endif

#define UART_RX_BUFSZ 8

/* STM32 uart driver */
struct stm32_uart
{
    struct rt_device parent;
    struct rt_ringbuffer rx_rb;
    USART_TypeDef * uart_base;
    IRQn_Type uart_irq;
    rt_uint8_t rx_buffer[UART_RX_BUFSZ];
    
};
#ifdef RT_USING_UART1
struct stm32_uart uart1_device;
#endif

#ifdef RT_USING_UART2
struct stm32_uart uart2_device;
#endif

void uart_irq_handler(struct stm32_uart* uart)
{
    /* enter interrupt */
    rt_interrupt_enter();
    
    if(LL_USART_IsActiveFlag_RXNE(uart->uart_base))// RXIRQ
    {
        rt_ringbuffer_putchar_force(&(uart->rx_rb), (rt_uint8_t)LL_USART_ReceiveData8(uart->uart_base));
        /* invoke callback */
        if(uart->parent.rx_indicate != RT_NULL)
        {
            uart->parent.rx_indicate(&uart->parent, rt_ringbuffer_data_len(&uart->rx_rb));
        }    
    }
    
    /* leave interrupt */
    rt_interrupt_leave();
}


#ifdef RT_USING_UART1
void USART1_IRQHandler(void)
{
    uart_irq_handler(&uart1_device);
}
#endif
#ifdef RT_USING_UART2
void USART2_IRQHandler(void)
{
    uart_irq_handler(&uart2_device);
}
#endif

static void uart1_io_init(USART_TypeDef * uart_base)
{
#ifdef RT_USING_UART1
    if (uart_base == USART1)
    {
        LL_GPIO_InitTypeDef GPIO_InitStruct;

        /* Peripheral clock enable */
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

        /**USART1 GPIO Configuration  
        PB6   ------> USART1_TX
        PA10   ------> USART1_RX 
        */
        GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
        GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
        LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
        GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    else
#endif

#ifdef RT_USING_UART2
    if (uart_base == USART2)
    {
        LL_GPIO_InitTypeDef GPIO_InitStruct;

        /* Peripheral clock enable */
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

        /**USART2 GPIO Configuration  
        PA2   ------> USART2_TX
        PA3   ------> USART2_RX 
        */
        GPIO_InitStruct.Pin = LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
        GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    else
#endif
    {
        RT_ASSERT((uart_base == USART1) || (uart_base == USART2));
    }
}

static void uart_ll_init(USART_TypeDef * uart)
{
    LL_USART_InitTypeDef USART_InitStruct;

    USART_InitStruct.BaudRate = 115200;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(uart, &USART_InitStruct);

    LL_USART_DisableOverrunDetect(uart);
    LL_USART_ConfigAsyncMode(uart);
    LL_USART_EnableIT_RXNE(uart);
    LL_USART_Enable(uart);
}

static rt_err_t rt_uart_init (rt_device_t dev)
{
    struct stm32_uart* uart;
    RT_ASSERT(dev != RT_NULL);
    uart = (struct stm32_uart *)dev;
    
    uart1_io_init(uart->uart_base);
    uart_ll_init(uart->uart_base);

    return RT_EOK;
}

static rt_err_t rt_uart_open(rt_device_t dev, rt_uint16_t oflag)
{
    struct stm32_uart* uart;
    RT_ASSERT(dev != RT_NULL);
    uart = (struct stm32_uart *)dev;

    if (dev->flag & RT_DEVICE_FLAG_INT_RX)
    {
        /* Enable the UART Interrupt */
        NVIC_EnableIRQ(uart->uart_irq);
    }

    return RT_EOK;
}

static rt_err_t rt_uart_close(rt_device_t dev)
{
    struct stm32_uart* uart;
    RT_ASSERT(dev != RT_NULL);
    uart = (struct stm32_uart *)dev;

    if (dev->flag & RT_DEVICE_FLAG_INT_RX)
    {
        /* Disable the UART Interrupt */
        NVIC_DisableIRQ(uart->uart_irq);
    }

    return RT_EOK;
}

static rt_size_t rt_uart_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    rt_size_t length;
    struct stm32_uart* uart;
    RT_ASSERT(serial != RT_NULL);
    uart = (struct stm32_uart *)dev;
    /* interrupt receive */
    rt_base_t level;

    RT_ASSERT(uart != RT_NULL);

    /* disable interrupt */
    level = rt_hw_interrupt_disable();
    length = rt_ringbuffer_get(&(uart->rx_rb), buffer, size);
    /* enable interrupt */
    rt_hw_interrupt_enable(level);

    return length;
}

static rt_size_t rt_uart_write(rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    char *ptr = (char*) buffer;
    struct stm32_uart* uart;
    RT_ASSERT(serial != RT_NULL);
    uart = (struct stm32_uart *)dev;

    if (dev->open_flag & RT_DEVICE_FLAG_STREAM)
    {
        /* stream mode */
        while (size)
        {
            if (*ptr == '\n')
            {
                while (!LL_USART_IsActiveFlag_TXE(uart->uart_base));
                LL_USART_TransmitData8(uart->uart_base, '\r');   
            }

            while (!LL_USART_IsActiveFlag_TXE(uart->uart_base));
            LL_USART_TransmitData8(uart->uart_base, *ptr); 

            ptr ++;
            size --;
        }
    }
    else
    {
        while (size)
        {
            while (!LL_USART_IsActiveFlag_TXE(uart->uart_base));
            LL_USART_TransmitData8(uart->uart_base, *ptr); 

            ptr++;
            size--;
        }
    }

    return (rt_size_t) ptr - (rt_size_t) buffer;
}

int rt_hw_usart_init(void)
{
    
#ifdef RT_USING_UART1
    {
        struct stm32_uart* uart;

        /* get uart device */
        uart = &uart1_device;

        /* device initialization */
        uart->parent.type = RT_Device_Class_Char;
        uart->uart_base = USART1;
        uart->uart_irq = USART1_IRQn;
        
        rt_ringbuffer_init(&(uart->rx_rb), uart->rx_buffer, sizeof(uart->rx_buffer));

        /* device interface */
        uart->parent.init 	    = rt_uart_init;
        uart->parent.open 	    = rt_uart_open;
        uart->parent.close      = rt_uart_close;
        uart->parent.read 	    = rt_uart_read;
        uart->parent.write      = rt_uart_write;
        uart->parent.control    = RT_NULL;
        uart->parent.user_data  = RT_NULL;

        rt_device_register(&uart->parent, "uart1", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    }
#endif

#ifdef RT_USING_UART2
    {
        struct stm32_uart* uart;

        /* get uart device */
        uart = &uart2_device;

        /* device initialization */
        uart->parent.type = RT_Device_Class_Char;
        uart->uart_base = USART2;
        uart->uart_irq = USART2_IRQn;
        rt_ringbuffer_init(&(uart->rx_rb), uart->rx_buffer, sizeof(uart->rx_buffer));

        /* device interface */
        uart->parent.init 	    = rt_uart_init;
        uart->parent.open 	    = rt_uart_open;
        uart->parent.close      = rt_uart_close;
        uart->parent.read 	    = rt_uart_read;
        uart->parent.write      = rt_uart_write;
        uart->parent.control    = RT_NULL;
        uart->parent.user_data  = RT_NULL;

        rt_device_register(&uart->parent, "uart2", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    }
#endif /* RT_USING_UART2 */
    return 0;
}
INIT_BOARD_EXPORT(rt_hw_usart_init);

#endif /*RT_USING_UART*/
