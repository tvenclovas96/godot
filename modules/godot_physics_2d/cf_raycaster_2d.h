#ifndef CF_RAYCAST_2D_H
#define CF_RAYCAST_2D_H

#include "core/templates/a_hash_map.h"
#include "core/templates/local_vector.h"
#include "modules/godot_physics_2d/godot_space_2d.h"
#include "servers/physics_server_2d.h"

// holder of cache-friendly raycast equivalent that is processed internally in the physics server during the physics step
// as opposed to a query that the regular raycast api does
class CFRaycaster2D {
public:
	_FORCE_INLINE_ RID create_ray() {
		RID barely_a_rid = RID::from_uint64(++rid_counter);

		ray_data.insert_new(barely_a_rid, CFRaycastData());
		result_data.insert_new(barely_a_rid, CFRaycastResult());
		return barely_a_rid;
	}

	_FORCE_INLINE_ bool clear_ray(const RID &p_rid) {
		bool ray = ray_data.erase(p_rid);
		bool result = result_data.erase(p_rid);

		ERR_FAIL_COND_V(ray != result, false); // should never happen, but idk

		if (ray_data.size() == 0) {
			rid_counter = 0; // reset when empty
		}

		return ray && result;
	}

	_FORCE_INLINE_ CFRaycastData *get_data_ptr(const RID &p_rid) {
		return ray_data.getptr(p_rid);
	}
	_FORCE_INLINE_ CFRaycastResult *get_result_ptr(const RID &p_rid) {
		return result_data.getptr(p_rid);
	}

	void intersect_rays(GodotSpace2D *p_space);

private:
	uint64_t rid_counter = 0; // just always increment

	// initialize with decent starting capacity
	AHashMap<RID, CFRaycastData> ray_data{ 1000 };
	AHashMap<RID, CFRaycastResult> result_data{ 1000 };

	GodotPhysicsDirectSpaceState2D *current_space = nullptr;

	void _intersect(uint32_t i);

public:
	CFRaycaster2D() {}
	//~CFRaycaster2D() {}
};

#endif //CF_RAYCAST_2D_H
