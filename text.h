#pragma once

#include <glad/glad.h>
#include <stdint.h>

#include <text_data.h>

#include "glutils.h"

typedef struct text_s {
	Mesh mesh;
	float width;
	vec3 pos;
	float scale;
} Text;

typedef enum verticalAnchor_e {
	TOP_ANCHOR, MIDDLE_ANCHOR, BOTTOM_ANCHOR
} VerticalAnchor;

typedef enum horizontalAnchor_e {
	RIGHT_ANCHOR, CENTER_ANCHOR, LEFT_ANCHOR
} HorizontalAnchor;

Text createText(const uint8_t* restrict const text, float scale);

void fixHorizontal(Text* restrict text, HorizontalAnchor anchor, float distance);
void fixVertical(Text* restrict text, VerticalAnchor anchor, float distance);