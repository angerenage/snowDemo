#include "text.h"

#include <string.h>
#include <stdlib.h>

#include "cameraController.h"

typedef struct charSquare_s {
	vec2 p[4];
	unsigned int i[6];
	int id;
} CharSquare;

static CharSquare *createCharacter(const Glyph g, int charId, int* restrict squareNumber) {
	CharSquare *squares = (CharSquare*)malloc(sizeof(CharSquare) * 5 * 8);
	int squareNum = 0;

	if (squares) {
		for (int row = 0; row < 8; row++) {
			for (int column = 0; column < 5; column++) {
				if (g[column] & (1 << row)) {
					squares[squareNum].id = charId * 5 * 8 + squareNum;

					vec2 origin = (vec2){(column + 6 * charId) * 0.11f, row * -0.11f};
					squares[squareNum].p[0] = origin;
					squares[squareNum].p[1] = (vec2){origin.x + 0.1f, origin.y};
					squares[squareNum].p[2] = (vec2){origin.x, origin.y - 0.1f};
					squares[squareNum].p[3] = (vec2){origin.x + 0.1f, origin.y - 0.1f};

					squares[squareNum].i[0] = 2;
					squares[squareNum].i[1] = 1;
					squares[squareNum].i[2] = 0;
					squares[squareNum].i[3] = 1;
					squares[squareNum].i[4] = 2;
					squares[squareNum].i[5] = 3;

					squareNum++;
				}
			}
		}
	}

	*squareNumber = squareNum;
	return squares;
}

Text createText(const uint8_t* restrict const text, float scale) {
	Text result = {
		.width = strlen((const char*)text) * 0.66f,
		.pos = (vec3){0.0, 0.0, 0.0},
		.scale = scale,
	};

	int charId = 0;
	unsigned int totalSquareCount = 0;

	vec3 *points = NULL;
	unsigned int *indices = NULL;
	unsigned int pointCount = 0;
	unsigned int indexCount = 0;

	while (text[charId] != '\0') {
		int squareNumber = 0;
		CharSquare *squares = createCharacter(glyph_data[text[charId] - 1], charId, &squareNumber);
		if (squareNumber > 0 && squares) {
			points = realloc(points, (pointCount + squareNumber * 4) * sizeof(vec3));
			indices = realloc(indices, (indexCount + squareNumber * 6) * sizeof(unsigned int));

			for (int i = 0; i < squareNumber; i++) {
				for (int j = 0; j < 4; j++) { // Each square has 4 vertices
					points[pointCount].x = squares[i].p[j].x;
					points[pointCount].y = squares[i].p[j].y;
					points[pointCount].z = (float)squares[i].id;
					pointCount++;
				}
				for (int k = 0; k < 6; k++) { // Each square has 6 indices
					indices[indexCount++] = squares[i].i[k] + totalSquareCount * 4;
				}
				totalSquareCount++;
			}
			free(squares);
		}

		charId++;
	}

	GLuint vao = createIndexedVAO(points, pointCount, indices, indexCount);
	free(points);
	free(indices);

	result.mesh = (Mesh){vao, pointCount, indexCount};
	return result;
}

void fixHorizontal(Text* restrict text, HorizontalAnchor anchor, float distance) {
	float textWidth = (text->width * text->scale) - (0.11f * text->scale);
	distance /= screenSize.x;

	switch (anchor) {
		default:
		case LEFT_ANCHOR:
			text->pos.x = distance - 1.0f;
			break;

		case CENTER_ANCHOR:
			text->pos.x = distance - (textWidth / 2.0f);
			break;

		case RIGHT_ANCHOR:
			text->pos.x = 1.0f - distance - textWidth;
			break;
	}
}

void fixVertical(Text* restrict text, VerticalAnchor anchor, float distance) {
	float textHeight = ((0.88f * text->scale * screenSize.y) * (screenSize.x / screenSize.y)) / screenSize.y;
	distance /= screenSize.y;

	switch (anchor) {
		default:
		case TOP_ANCHOR:
			text->pos.y = 1.0f - distance;
			break;

		case MIDDLE_ANCHOR:
			text->pos.y = distance + (textHeight / 2.0f);
			break;

		case BOTTOM_ANCHOR:
			text->pos.y = distance + textHeight - 1.0f;
			break;
	}
}