#pragma once

#include "uEye.h"

#include "includes.hpp"
#include "error.hpp"

namespace ids
{
	// 
	class capture_status_property
	{
	public:
		// 
		capture_status_property(HIDS camera_handle) :
			camera_handle(camera_handle)
		{

		}

		// Gets the current capture status.
		UEYE_CAPTURE_STATUS_INFO get() const
		{
			UEYE_CAPTURE_STATUS_INFO capture_status_info;
			INT result = is_CaptureStatus(
				this->camera_handle,
				IS_CAPTURE_STATUS_INFO_CMD_GET,
				&capture_status_info,
				sizeof(capture_status_info));
			detail::throw_on_error(this->camera_handle, result);

			return capture_status_info;
		}

		// Resets error counters.
		void reset() const
		{
			INT result = is_CaptureStatus(
				this->camera_handle,
				IS_CAPTURE_STATUS_INFO_CMD_RESET,
				nullptr,
				0);
			detail::throw_on_error(this->camera_handle, result);
		}

	private:
		const HIDS camera_handle;
	};
}