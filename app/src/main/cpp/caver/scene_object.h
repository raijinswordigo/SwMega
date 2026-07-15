#ifndef NEEDLE_SCENE_OBJECT_H
#define NEEDLE_SCENE_OBJECT_H

#include "lua.h"
#include "core/cpp_strings.h"
#include "core/hooks.h"
#include "types.h"

// Credits Kizi btw

typedef struct Component Component; // Needs mapping

typedef struct SceneObject {
	char _pad0[archSplit(0x10, 0x20)];
	void *scene; // Scene*
	void *groups; // FastVector<SceneObjectGroup*>*
	char _pad1[archSplit(0x8, 0x10)];
	float speed;
	char _pad1b[0x4];
	void *object_template; // ObjectTemplate*
	CppString *identifier;
	void *program; // boost::shared_ptr<Program>.px
	void *program_refcount; // boost::shared_ptr<Program>.pn
	Vector2 velocity;
	Vector3 position;
	char _pad_unk[0x4];
	float rotation;
	float angular_velocity;
	float instance_scale;
	float world_scale;
	char flip_x;
	char _pad3d[0x3];
	Rectangle local_aabb; // {xf, yf, wf, hf};
	char _pad4[0x10];
	char bounds_registered;
	char _pad5a[0xB];
	Component **components_begin;
	Component **components_end;
	Component **components_cap_end;
	char _pad5b[archSplit(0x1C, 0x24)];
	char alwaysActive;
} SceneObject;

SceneObject *scene_object_from_L(lua_State *L, int idx);
CppString *scene_object_get_identifier(SceneObject *obj);
float scene_object_get_speed(SceneObject *obj);
void scene_object_set_speed(SceneObject *obj, float speed);

#endif // NEEDLE_SCENE_OBJECT_H
