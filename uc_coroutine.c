#include "uc_coroutine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct Generator_Stack generator_stack;

void generator_return(void *arg, void *rsp);
void __attribute__((naked)) generator_restore_context_with_return(void *arg, void *rsp);

void generator_switch_context(struct Generator *g, void *rsp, void *arg);
void __attribute__((naked)) generator_restore_context(void *rsp);

void da_append(struct Generator_Stack *gs, struct Generator *g)
{
        assert(gs != NULL && g != NULL);

        if (gs->count >= gs->capacity) {
                uint32_t new_capacity = (gs->capacity == 0) ? 4 : gs->capacity * 2;

                struct Generator **new_items = realloc(gs->items, new_capacity * sizeof(struct Generator *));
                if (new_items == NULL) {
                    return;
                }
                gs->items = new_items;
                gs->capacity = new_capacity;
        }
        gs->items[gs->count] = g;
        gs->count++;
}

void generator_init(void)
{
        struct Generator *g = malloc(sizeof(struct Generator));
        assert(g != NULL && "Buy more RAM lol");
        memset(g, 0, sizeof(struct Generator));
        da_append(&generator_stack, g);
}

void *__attribute__((naked)) generator_yield(void *arg)
{
        __asm__(
        
                "push {lr}\n"
                
                "push {r0}\n"
                "push {r1}\n"
                "push {r2}\n"
                "push {r3}\n"
        
                "push {r4}\n"
                "push {r5}\n"
                "push {r6}\n"
                "push {r7}\n"
        
                "mov r1, sp\n"
                "bl generator_return\n"
        );
}

void generator_return(void *arg, void *rsp)
{
        generator_stack.items[generator_stack.count - 1]->rsp = rsp;
        generator_stack.count -= 1;
        generator_restore_context_with_return(arg, generator_stack.items[generator_stack.count - 1]->rsp);
}

void __attribute__((naked)) generator_restore_context_with_return(void *arg, void *rsp)
{
        __asm__(
                "mov sp, r1\n"
        
                "pop {r7}\n"
                "pop {r6}\n"
                "pop {r5}\n"
                "pop {r4}\n"
        
                "pop {r3}\n"
                "pop {r2}\n"
                "pop {r1}\n"
                "mov lr, r0\n"
                "pop {r0}\n"
                "mov r0, lr\n"
        
                "pop {pc}\n"
        );
}

void *__attribute__((naked)) generator_next(struct Generator *g, void *arg)
{
        __asm__(
                "push {lr}\n"
        
                "push {r0}\n"
                "push {r1}\n"
                "push {r2}\n"
                "push {r3}\n"
        
                "push {r4}\n"
                "push {r5}\n"
                "push {r6}\n"
                "push {r7}\n"
        
                "mov r2, sp\n"
                "bl generator_switch_context\n"
        );
}

void generator_switch_context(struct Generator *g, void *arg, void *rsp)
{
        generator_stack.items[generator_stack.count - 1]->rsp = rsp;
        da_append(&generator_stack, g);
        if (g->fresh) {
                g->fresh = false;
                void **rsp = (void**)((uint8_t*)g->stack_base + STACK_CAPACITY);
                //******************************************
                //^                                        ^
                //g->stack_base                            rsp
                *(rsp - 3) = arg;
        }
        generator_restore_context(g->rsp);
        /*
                ************************************
                ^                        ^
                jump                     func
                When entering func for the first time,
                it will be pushed into the stack lr register,
                and the lr value is here
                so we just pop complete func to over all things
        */
        __asm__("pop {pc}\n");
}

void __attribute__((naked)) generator_restore_context(void *rsp)
{
        __asm__(
                "mov sp, r0\n"
                
                "pop {r7}\n"
                "pop {r6}\n"
                "pop {r5}\n"
                "pop {r4}\n"
        
                "pop {r3}\n"
                "pop {r2}\n"
                "pop {r1}\n"
                "pop {r0}\n"
                
                "pop {pc}\n"
        );
}

void generator_finish_current(void)
{
        generator_stack.items[generator_stack.count - 1]->dead = true;
        generator_stack.count -= 1;
        generator_restore_context_with_return(NULL, generator_stack.items[generator_stack.count - 1]->rsp);
}

struct Generator *generator_create(void (*task)(void *))
{
        struct Generator *g = malloc(sizeof(struct Generator));
        assert(g != NULL && "Buy more RAM lol");
        memset(g, 0, sizeof(struct Generator));
        g->stack_base = malloc(STACK_CAPACITY);
#if CHECK_STACK_OVERFLOW
        memset(g->stack_base, 0xa5U, STACK_CAPACITY);
#endif
        void **rsp = (void**)((uint8_t*)g->stack_base + STACK_CAPACITY);
        *(--rsp) = generator_finish_current;
        /* lr */
        *(--rsp) = task;
        /* r0 - r7 */
        *(--rsp) = 0;
        *(--rsp) = 0;
        *(--rsp) = 0;
        *(--rsp) = 0;
        
        *(--rsp) = 0;
        *(--rsp) = 0;
        *(--rsp) = 0;
        *(--rsp) = 0;
        
        g->rsp = rsp;
        g->fresh = true;
        return g;
}

void generator_destory(struct Generator *g)
{
        memset(g->stack_base, 0, STACK_CAPACITY);
        free(g->stack_base);
        g->stack_base = NULL;
        
        memset(g, 0, sizeof(struct Generator));
        free(g);
        g = NULL;
}
