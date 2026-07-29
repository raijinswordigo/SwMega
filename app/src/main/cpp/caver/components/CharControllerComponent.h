#ifndef MEGA_CHARCONTROLLERCOMPONENT_H
#define MEGA_CHARCONTROLLERCOMPONENT_H

#include "core/hooks.h"

#include <stdint.h>

typedef struct CharControllerComponent {
	char _base[archSplit(0x38, 0x3C)];

	void *entity;
	void *default_animation_controller;
	void *lift_animation_controller;

	char _pad1[archSplit(0x48, 0x54)];

	void *lift_animation;
	void *drop_animation;
	void *throw_animation;
	void *push_animation;
	void *hurt_animation;
	void *die_animation;

	void *jump_sound;
	void *air_jump_sound;
	void *jump_land_sound;

	void *right_weapon_controller;
	void *left_weapon_controller;

	void *swing1;
	void *swing2;
	void *swing3;

	char _pad2[archSplit(0x24, 0x28)];

	float max_fall_distance; // 0x118/0x220

	char _pad3[archSplit(0x10, 0x14)];

	void *CharAnimController; // 0xCC/0x1D4

	char _pad4[archSplit(0x65, 0x69)];

	int forceFaceDirectionDisable; // 0x13D/0x245

	char _pad5[archSplit(0xF, 0xF)];

	float max_climb_height; // 0x150/0x258

	char _pad6[archSplit(0x10, 0x10)];

	float jump_speed; // 0x164/0x26C
	float walk_speed; // 0x170/0x278
	float air_control; // 0x174/0x27C
	float run_speed; // 0x178/0x280
	float run_accel; // 0x17C/0x284

	char _pad7[archSplit(0x5C, 0x74)];

	uint32_t flags; // 0x1D8/0x2F8
	uint32_t state; // 0x1DC/0x300
	uint16_t input; // 0x1E0/0x304
} CharControllerComponent;

#endif //MEGA_CHARCONTROLLERCOMPONENT_H
