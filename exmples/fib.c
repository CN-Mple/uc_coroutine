#include "../uc_coroutine.h"

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

void main(void)
{
        generator_init();
        struct Generator *g = generator_create(fib);

        void *value = generator_next(g, (void*)(1000 * 1000));
        while (true) {
                if(g->dead) break;
                printf("[%d]\r\n", (int)value);
                value = generator_next(g, (void*)0);
        }
        generator_destory(g);
}
