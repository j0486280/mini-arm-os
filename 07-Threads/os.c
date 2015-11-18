#include <stddef.h>
#include <stdint.h>
#include "reg.h"
#include <string.h>
#include <stdlib.h>
#include "threads.h"

/* USART TXE Flag
 * This flag is cleared when data is written to USARTx_DR and
 * set when that data is transferred to the TDR
 */
#define USART_FLAG_TXE	((uint16_t) 0x0080)

#define USART_FLAG_RXNE ((uint16_t) 0x0020)

extern int fibonacci(int x);

void usart_init(void)
{
	*(RCC_APB2ENR) |= (uint32_t) (0x00000001 | 0x00000004);
	*(RCC_APB1ENR) |= (uint32_t) (0x00020000);

	/* USART2 Configuration, Rx->PA3, Tx->PA2 */
	*(GPIOA_CRL) = 0x00004B00;
	*(GPIOA_CRH) = 0x44444444;
	*(GPIOA_ODR) = 0x00000000;
	*(GPIOA_BSRR) = 0x00000000;
	*(GPIOA_BRR) = 0x00000000;

	*(USART2_CR1) = 0x0000000C;
	*(USART2_CR2) = 0x00000000;
	*(USART2_CR3) = 0x00000000;
	*(USART2_CR1) |= 0x2000;
}

void print_str(const char *str)
{
	while (*str) {
		while (!(*(USART2_SR) & USART_FLAG_TXE));
		*(USART2_DR) = (*str & 0xFF);
		str++;
	}
}

void print_char(const char *str)
{
	if(*str){
		while (!(*(USART2_SR) & USART_FLAG_TXE));
		*(USART2_DR) = (*str & 0xFF);
	}
}

char *itoa(uint32_t val)
{
	static char buf[33]= {0};
	buf[9] = '\0';
	char *index = buf + 32;
	
	do
	{
		*(--index) = (val%10) + '0';
		val /= 10;
	}
	while( val > 0 );

	return index;
}

char scan_char()
{
	while (1) {
		while (!(*(USART2_SR) & USART_FLAG_RXNE));
		return ((*USART2_DR) & 0xFF);
	}
}

static void delay(volatile int count)
{
	count *= 50000;
	while (count--);
}

static void busy_loop(void *str)
{
	while (1) {
		print_str(str);
		print_str(": Running...\n");
		delay(1000);
	}
}

void fib(void *in)
{
	int x = (int) in;
	uint32_t val = fibonacci(x);
	print_str("Fibonacci (");
	print_str(itoa(x));
	print_str(") is ");
	print_str(itoa(val));
	print_str("\n");

}

void decode(char *input)
{
	char *tok = strtok(input," ");

	if(tok[0] == 'f' && tok[1] == 'i' && tok[2] == 'b' && tok[3] == '\0'){
		
		tok = strtok(NULL," ");
		int val = atoi(tok);
		//const int val = 10;
		fib((void *) val);
		while(tok != NULL)
			tok = strtok(NULL," ");
	} else {
		print_str("Function is not found...\n");
	}
}

void shell()
{
	char buf[128];
	int index;
	while(1){
		print_str("embedded@embedded-PC$");
		index = 0;
		while(1){
			buf[index] = scan_char();
			buf[(index+1)] = '\0';
			
			if( buf[index] == 13 || buf[index] == '\n' ){
				print_char("\n");
				decode(buf);
				break;
			}
			else if(buf[index] == 8 || buf[index] == 127){
				if(index > 0){
					buf[index] = '\0';
					index--;
					print_str("\b \b");
				}
			}
			else{
				print_char(&buf[index]);
				index++;
			}
		}
	}
}

void test1(void *userdata)
{
	busy_loop(userdata);
}

void test2(void *userdata)
{
	busy_loop(userdata);
}

void test3(void *userdata)
{
	busy_loop(userdata);
}

/* 72MHz */
#define CPU_CLOCK_HZ 72000000

/* 100 ms per tick. */
#define TICK_RATE_HZ 10

int main(void)
{
	usart_init();

	if (thread_create(shell, (void *) NULL) == -1)
		print_str("Shell creation failed\r\n");

	/* SysTick configuration */
	*SYSTICK_LOAD = (CPU_CLOCK_HZ / TICK_RATE_HZ) - 1UL;
	*SYSTICK_VAL = 0;
	*SYSTICK_CTRL = 0x07;

	thread_start();

	return 0;
}
