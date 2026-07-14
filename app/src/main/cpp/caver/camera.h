#ifndef KIWI_CAMERA_H
#define KIWI_CAMERA_H

#include "core/hooks.h"
#include "types.h"

typedef struct CameraController {
	int mode;

	Vector3 offset;

	Vector3 targetPosition;
	float positionSmoothing;

	Vector3 targetLookAt;
	float lookAtSmoothing;

	char _pad0[0x18];

	Vector3 upVector;

	void *camera;

	char _pad1[archSplit(0x28, 0x34)];

	float shakeElapsed;
	float shakeDuration;

} CameraController;

#endif
