#pragma once

#include "uEye.h"

#include "error.hpp"
#include "includes.hpp"

namespace ids
{
	// 
	class exposure_property
	{
	public:
		//
		exposure_property(HIDS camera_handle) :
			camera_handle(camera_handle)
		{

		}

		// Gets the default exposure time in microseconds.
		uint32_t default() const
		{
			return this->get<IS_EXPOSURE_CMD_GET_EXPOSURE_DEFAULT>();
		}

		// Gets the maximumm exposure time in microseconds.
		uint32_t max() const
		{
			return this->get<IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MAX>();
		}

		// Gets the minimum exposure time in microseconds.
		uint32_t min() const
		{
			return this->get<IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MIN>();
		}


		// Gets the exposure time increment in microseconds.
		uint32_t inc() const
		{
			return this->get<IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_INC>();
		}

		// Gets the current exposure time in microseconds.
		uint32_t get() const
		{
			return this->get<IS_EXPOSURE_CMD_GET_EXPOSURE>();
		}

		// Sets the current exposure time in microseconds.
		void set(uint32_t value) const
		{
			float64_t exposure = value / 1000.0;
			INT result = is_Exposure(
				this->camera_handle,
				IS_EXPOSURE_CMD_SET_EXPOSURE,
				&exposure,
				sizeof(exposure));
			detail::throw_on_error(this->camera_handle, result);
		}

	private:
		template<UINT command>
		uint32_t get() const
		{
			float64_t exposure;
			INT result = is_Exposure(
				this->camera_handle,
				command,
				&exposure,
				sizeof(exposure));
			detail::throw_on_error(this->camera_handle, result);
			return static_cast<uint32_t>(exposure * 1000);
		}

		const HIDS camera_handle;
	};
}