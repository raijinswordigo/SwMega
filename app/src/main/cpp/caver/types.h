#ifndef KIWI_TYPES_H
#define KIWI_TYPES_H

typedef struct Vector3 {
	float x;
	float y;
	float z;
} Vector3;

typedef struct Vector2 {
	float x;
	float y;
} Vector2;

typedef struct Quaternion {
	float x;
	float y;
	float z;
	float w;
} Quaternion;

typedef struct Matrix4 {
	float m[16];
} Matrix4;

typedef struct FloatColor {
	float R;
	float G;
	float B;
	float A;
} FloatColor;

typedef struct Rectangle {
	float x;
	float y;
	float w;
	float h;
} Rectangle;

#endif // KIWI_TYPES_H
