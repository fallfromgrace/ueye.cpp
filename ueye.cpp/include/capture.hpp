#pragma once

#include "uEye.h"

#include "includes.hpp"
#include "error.hpp"

namespace ids
{
	// Controls the status of camera capture.
	class capture_property
	{
	public:
		// 
		capture_property(HIDS camera_handle) :
			camera_handle(camera_handle)
		{

		}

		// 
		bool_t get() const
		{
			INT result = is_CaptureVideo(this->camera_handle, IS_GET_LIVE);
			return result == 1;
		}

		// 
		void start()
		{
			INT result = is_CaptureVideo(this->camera_handle, IS_WAIT);
			detail::throw_on_error(this->camera_handle, result);
		}

		// 
		void stop()
		{
			INT result = is_StopLiveVideo(this->camera_handle, IS_FORCE_VIDEO_STOP);
			detail::throw_on_error(this->camera_handle, result);
		}

		// 
		void snap()
		{
			INT result = is_FreezeVideo(this->camera_handle, IS_DONT_WAIT);
			detail::throw_on_error(this->camera_handle, result);
		}
	private:
		const HIDS camera_handle;
	};
}