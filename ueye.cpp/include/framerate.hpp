#pragma once

#include "uEye.h"

#include "includes.hpp"
#include "error.hpp"

namespace ids
{
	// 
	class framerate_property
	{
	public:
		// 
		framerate_property(HIDS camera_handle) :
			camera_handle(camera_handle)
		{

		}

		// 
		int32_t actual() const
		{
			double framerate;
			INT result = is_GetFramesPerSecond(this->camera_handle, &framerate);
			detail::throw_on_error(this->camera_handle, result);
			return static_cast<int32_t>(framerate * 1000);
		}

		// 
		int32_t get() const
		{
			double framerate;
			INT result = is_SetFrameRate(
				this->camera_handle,
				static_cast<double>(IS_GET_FRAMERATE),
				&framerate);
			detail::throw_on_error(this->camera_handle, result);

			return static_cast<int32_t>(framerate * 1000);
		}

		// 
		void set(int32_t value) const
		{
			double framerate = value / 1000.0;
			INT result = is_SetFrameRate(
				this->camera_handle,
				framerate,
				&framerate);
			detail::throw_on_error(this->camera_handle, result);
		}

	private:
		const HIDS camera_handle;
	};
}