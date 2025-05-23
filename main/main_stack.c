#include <stdio.h>
#include "uc_coroutine.h"

int point = 0;
char debug_window[1024];

int fputc(int ch, FILE *f)
{
        (void)f;
        debug_window[((point++) % 1024)] = (char)ch;
        return ch;
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
        generator_init();

        struct Generator *g = generator_create(fib);
        foreach(value, g, (void*)(1000 * 1000))
                printf("[%d]\r\n", (int)value);
        generator_destory(g);
        (void)g;

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
        while (1)
        {
                
        }
}
