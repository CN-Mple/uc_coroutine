#include "uc_coroutine_stackless.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Generator* generator_stack[STACK_CAPACITY] = {0};

static uint32_t coroutine_uuid = 0;
static uint32_t coroutine_index = 0;

void generator_init(void)
{
	memset(generator_stack, 0, sizeof(generator_stack));
	coroutine_uuid = 0;
	coroutine_index = 0;
}

struct Generator* generator_create(void (*task)(void*))
{
	struct Generator *g = malloc(sizeof(struct Generator));
	g->uuid = coroutine_uuid;
	g->line = 0;
	g->state = COROUTINE_SUSPENDED;
	g->task = task;
	generator_stack[coroutine_uuid++] = g;
	return g;
}

void generator_next(struct Generator *g)
{
	g->state = COROUTINE_RUNNING;

	if (g->task) {
		g->task(g);
	}
}

void generator_destory(struct Generator *g)
{
	generator_stack[g->uuid] = NULL;
	memset(g, 0, sizeof(struct Generator));
	free(g);
	g = NULL;
}

void generator_schedule(void)
{
	struct Generator *g = generator_stack[coroutine_index++ % STACK_CAPACITY];

	if (g == NULL) {
		return;
	}

	if (COROUTINE_SUSPENDED == g->state) {
		generator_next(g);
	}

	if (COROUTINE_DEADED == g->state) {
		generator_destory(g);
	}
}