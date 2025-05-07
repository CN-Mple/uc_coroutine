#ifndef __UC_COROUTINE_H
#define __UC_COROUTINE_H

#include <stdint.h>
#include <stdbool.h>

#if 0
#define TODO(message)           printf(message)
#else
#define TODO(message)           _Pragma("message \"TODO: " message "\"")
#endif

#define assert(x)               do{if(!(x)){printf("line is assert at [%d]",__LINE__);while(1);}}while(0)

#define STACK_CAPACITY          (128)
#define CHECK_STACK_OVERFLOW    (1)

#define foreach(it, g, arg)     for(void *it = generator_next(g, arg); (!g->dead); it = generator_next(g, arg))
#define do_next(it, g, arg)     do{ if(!g->dead) it = generator_next(g, arg); }while(0)
struct Generator {
        void *rsp;
        void *stack_base;
        bool dead;
        bool fresh;
};

struct Generator_Stack {
        struct Generator **items;
        uint32_t count;
        uint32_t capacity;
};

void generator_init(void);
struct Generator *generator_create(void (*task)(void *));
void *__attribute__((naked)) generator_yield(void *arg);
void *__attribute__((naked)) generator_next(struct Generator *g, void *arg);
void generator_destory(struct Generator *g);


#endif /* __UC_COROUTINE_H */
