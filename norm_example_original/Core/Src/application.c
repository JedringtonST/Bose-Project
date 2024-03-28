#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "application.h"
#include "ispu.h"

#include "i2c.h"
#include "tim.h"
#include "gpio.h"
#include "usart.h"

#define UART_BUF_SIZE 256

void read(uint8_t reg, uint8_t *val, uint16_t len);
void write(uint8_t reg, uint8_t val);

static char uart_char;
static uint8_t uart_received;
static char uart_buff[UART_BUF_SIZE + 1];
static uint16_t uart_size;

static volatile uint8_t enable_int;
static volatile uint8_t algo_int;
static volatile uint8_t sleep_int;
static uint32_t exec_time;

static uint16_t print_results;
static uint16_t print_time;

void application(void)
{
	HAL_Delay(1000);

	uint8_t who_am_i;
	uint32_t start = HAL_GetTick();
	do {
		if (HAL_GetTick() - start > 1000) { // retry for 1.0 s
			while (1) {
				printf("Error: adapter not recognized (%02x)\n", who_am_i);
				HAL_Delay(1000);
			}
		}
		write(0x01, 0x00); // set default registers access
		read(0x0F, &who_am_i, 1);
	} while (who_am_i != 0x22);

	write(0x12, 0x01); // software reset
	HAL_Delay(1);

	// load device configuration
	for (uint32_t i = 0; i < sizeof(ispu_conf) / sizeof(ucf_line_ext_t); i++) {
		if (ispu_conf[i].op == MEMS_UCF_OP_WRITE)
			write(ispu_conf[i].address, ispu_conf[i].data);
		else if (ispu_conf[i].op == MEMS_UCF_OP_DELAY)
			HAL_Delay(ispu_conf[i].data);
	}

	HAL_UART_Receive_IT(&huart2, (uint8_t *)&uart_char, 1);

	enable_int = 1;
	print_results = 1;

	while (1) {
		// handle commands received from uart
		if (uart_received) {
			if (sscanf(uart_buff, "res%hu", &print_results) > 0) {
				if (print_results)
					printf("Enabled results print.\n");
				else
					printf("Disabled results print.\n");
			}
			if (sscanf(uart_buff, "time%hu", &print_time) > 0) {
				if (print_time)
					printf("Enabled execution time print.\n");
				else
					printf("Disabled execution time print.\n");
			}

			uart_size = 0;
			uart_received = 0;
		}

		if (algo_int) {
			algo_int = 0;

			int16_t acc[3];
			float norm;

			write(0x01, 0x80);
			read(0x10, (uint8_t *)acc, 6);
			read(0x10 + 6, (uint8_t *)&norm, 4);
			write(0x01, 0x00);

			if (print_results)
				printf("%d\t%d\t%d\t%f\n", acc[0], acc[1], acc[2], norm);
		}

		if (sleep_int) {
			sleep_int = 0;

			if (print_time)
				printf("%lu\n", exec_time);
		}
	}
}

void read(uint8_t reg, uint8_t *val, uint16_t len)
{
	HAL_I2C_Mem_Read(&hi2c1, 0xD4, reg, I2C_MEMADD_SIZE_8BIT, val, len, 1000);
}

void write(uint8_t reg, uint8_t val)
{
	HAL_I2C_Mem_Write(&hi2c1, 0xD4, reg, I2C_MEMADD_SIZE_8BIT, &val, 1, 1000);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (uart_char == '\n') {
		uart_buff[uart_size] = '\0';
		uart_received = 1;
	} else if (uart_char == '*') {
		uart_size = 0;
	} else if (uart_char != '\r') {
		if (uart_size >= UART_BUF_SIZE)
			uart_size = 0;
		uart_buff[uart_size++] = uart_char;
	}

	HAL_UART_Receive_IT(&huart2, (uint8_t *)&uart_char, 1);
}

int _write(int fd, const void *buf, size_t count)
{
	uint8_t status = HAL_UART_Transmit(&huart2, (uint8_t *)buf, count, 1000);

	return (status == HAL_OK ? count : 0);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (enable_int) {
		switch (GPIO_Pin) {
		case INT1_Pin:
			algo_int = 1;
			break;
		case INT2_Pin:
			if (HAL_GPIO_ReadPin(INT2_GPIO_Port, INT2_Pin) == GPIO_PIN_RESET) {
				__HAL_TIM_SET_COUNTER(&htim5, 0);
				HAL_TIM_Base_Start(&htim5);
			} else if (HAL_GPIO_ReadPin(INT2_GPIO_Port, INT2_Pin) == GPIO_PIN_SET) {
				HAL_TIM_Base_Stop(&htim5);
				exec_time = __HAL_TIM_GET_COUNTER(&htim5);
				sleep_int = 1;
			}
		}
	}
}

