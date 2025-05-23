#ifndef __UC_COROUTINE_STACKLESS_H
#define __UC_COROUTINE_STACKLESS_H

#include <stdint.h>
#include <stdbool.h>

#if 0
#define TODO(message)           printf(message)
#else
#define TODO(message)           _Pragma("message \"TODO: " message "\"")
#endif

#define assert(x)               do{if(!(x)){printf("line is assert at [%d]",__LINE__);while(1);}}while(0)

#define STACK_CAPACITY          (16)

typedef enum {
	COROUTINE_DEADED = 0,
	COROUTINE_SUSPENDED,
	COROUTINE_RUNNING,
} coroutine_state;

struct Generator {
	uint32_t uuid;
	uint32_t line;
	coroutine_state state;
	void (*task)(void*);
};


#define generator_start()   switch (g->line) {case 0:
#define generator_stop()    g->state = COROUTINE_DEADED;break;}
#define generator_yield()   do {\
				    g->line = __LINE__;\
				    g->state = COROUTINE_SUSPENDED;\
				    case __LINE__:;\
				    if (g->state != COROUTINE_RUNNING) \
					return;\
				} while(0)

void generator_init(void);
struct Generator* generator_create(void (*task)(void*));
void generator_next(struct Generator *g);
void generator_destory(struct Generator *g);

void generator_schedule(void);

#endif /* __UC_COROUTINE_STACKLESS_H */
