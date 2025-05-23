#include <stdio.h>

#include "uc_coroutine_stackless.h"

int point = 0;
char debug_window[1024];

int fputc(int ch, FILE *f)
{
        (void)f;
        debug_window[((point++) % 1024)] = (char)ch;
        return ch;
}

void Coroutine_A(void *arg)
{
	struct Generator *g = (struct Generator *)arg;
	generator_start();
	/* init */
	printf("%-32s","A init\r\n");
	while (1) {
		/* do some thing */
		printf("%-32s","A do some thing pre\r\n");
		
		generator_yield();
		
		/* do some thing */
		printf("%-32s","A do some thing next\r\n");
		static uint8_t i = 3;
		if (i-- == 0) {
			break;
		}
	}
	generator_stop();
}

void Coroutine_B(void *arg)
{
	struct Generator *g = (struct Generator *)arg;
	generator_start();
	/* init */
	printf("%-32s","B init\r\n");
	while (1) {
		/* do some thing */
		printf("%-32s","B do some thing pre\r\n");
		
		generator_yield();
		
		/* do some thing */
		printf("%-32s","B do some thing next\r\n");
		static uint8_t i = 7;
		if (i-- == 0) {
			break;
		}
	}
	generator_stop();
}

int main(void)
{
	generator_create(Coroutine_A);
	generator_create(Coroutine_B);
	while (1) {
		generator_schedule();
	}
}
