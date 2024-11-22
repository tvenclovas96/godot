#include "cf_raycaster_2d.h"
#include "core/object/worker_thread_pool.h"

void CFRaycaster2D::intersect_rays(GodotSpace2D *p_space) {
	uint64_t profile_begtime = OS::get_singleton()->get_ticks_usec();
	uint64_t profile_endtime = 0;

	current_space = p_space->get_direct_state();
	// Currently, non-threaded approach is slightly faster (due to cache-coherency?)
	// test extensively once intersect logic is trimmed down
	// perhaps manually designating larger slices of the array to threads would work best

	// WorkerThreadPool::GroupID group_task = WorkerThreadPool::get_singleton()->add_template_group_task(this, &CFRaycaster2D::_intersect_threaded, nullptr, ray_data.size(), -1, true, SNAME("ThreadedIntersect"));
	// WorkerThreadPool::get_singleton()->wait_for_group_task_completion(group_task);

	for (uint32_t i = 0; i < ray_data.size(); i++) {
		_intersect_threaded(i, nullptr);
	}

	{ //profile
		profile_endtime = OS::get_singleton()->get_ticks_usec();
		p_space->set_elapsed_time(GodotSpace2D::ELAPSED_TIME_CAST_RAYS, profile_endtime - profile_begtime);
		profile_begtime = profile_endtime;
	}
}

void CFRaycaster2D::_intersect_threaded(uint32_t i, void *p_userdata) {
	CFRaycastData &data = ray_data.get_by_index(i).value;

	// collider data is reset only on explicit non-collision
	// so enabling a cfray for one frame will still provide info
	// a few frames later without the data being cleared. Use responsibly!

	if (!data.enabled) {
		return;
	}

	data.enabled = data.keep_enabled;

	CFRaycastResult &result = result_data.get_by_index(i).value;
	current_space->intersect_ray_lean(data, result);
}
