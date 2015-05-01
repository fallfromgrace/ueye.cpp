#pragma once

#include "uEye.h"
#include "includes.hpp"

#include "error.hpp"

namespace ids
{
	// 
	class pixel_clock_property
	{
	public:
		// 
		pixel_clock_property(HIDS camera_handle) :
			camera_handle(camera_handle)
		{

		}

		// Gets the default pixel clock frequency in MHz.
		uint32_t default() const
		{
			return this->get<IS_PIXELCLOCK_CMD_GET_DEFAULT>();
		}

		// Gets the current pixel clock frequency in MHz.
		uint32_t get() const
		{
			return this->get<IS_PIXELCLOCK_CMD_GET>();
		}

		// Sets the current pixel clock frequency in MHz.
		void set(uint32_t value) const
		{
			INT result = is_PixelClock(
				this->camera_handle,
				IS_PIXELCLOCK_CMD_SET,
				&value, 
				sizeof(value));
			detail::throw_on_error(this->camera_handle, result);
		}

	private:
		//
		template<UINT command>
		uint32_t get() const
		{
			UINT pixel_clock;
			INT result = is_PixelClock(
				this->camera_handle,
				command,
				&pixel_clock, sizeof(pixel_clock));
			detail::throw_on_error(this->camera_handle, result);
			return static_cast<uint32_t>(pixel_clock);
		}

		const HIDS camera_handle;
	};
}