#pragma once

#include <glad/glad.h>
#include <stdint.h>
#include <wchar.h>

#include "glutils.h"

typedef uint8_t Glyph[5];

typedef struct charSquare_s {
	vec2 p[4];
	unsigned int i[6];
	int id;
} CharSquare;

typedef struct text_s {
	wchar_t *text;
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

Text createText(wchar_t *text, float scale);

void fixHorizontal(Text* restrict text, HorizontalAnchor anchor, const vec2* restrict const screenSize, float distance);
void fixVertical(Text* restrict text, VerticalAnchor anchor, const vec2* restrict const screenSize, float distance);