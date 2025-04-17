#pragma once
#include <stddef.h>

typedef struct {
	void* data;
	size_t size;
} Ressource;

extern const Ressource res_running_anim;
