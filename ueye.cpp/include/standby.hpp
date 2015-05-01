#pragma once

#include "uEye.h"

#include "includes.hpp"
#include "error.hpp"

namespace ids
{
	// 
	class standby_property
	{
	public:
		//
		standby_property(HIDS camera_handle) :
			camera_handle(camera_handle)
		{

		}

		// Gets if the camera supports standby mode.
		bool_t supported() const
		{
			ULONG result = is_CameraStatus(
				this->camera_handle,
				IS_STANDBY_SUPPORTED,
				IS_GET_STATUS);
			//detail::throw_on_error(this->camera_handle, static_cast<INT>(result));
			return result == 1;
		}

		bool_t get() const
		{
			ULONG result = is_CameraStatus(
				this->camera_handle,
				IS_STANDBY,
				IS_GET_STATUS);
			//detail::throw_on_error(this->camera_handle, static_cast<INT>(result));
			return result == 1;
		}

		void set(bool_t value) const
		{
			ULONG result = is_CameraStatus(
				this->camera_handle,
				IS_STANDBY,
				static_cast<ULONG>(value));
			detail::throw_on_error(this->camera_handle, static_cast<INT>(result));
		}

	private:
		const HIDS camera_handle;
	};
}