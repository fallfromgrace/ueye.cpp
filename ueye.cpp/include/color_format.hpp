#pragma once

#include "uEye.h"

#include "includes.hpp"
#include "error.hpp"

namespace ids
{
	// 
	enum color_format
	{
		// 
		raw8,
		mono8
	};

	namespace detail
	{
		namespace convert
		{
			color_format from_ueye(INT mode)
			{
				switch (mode)
				{
				case IS_CM_SENSOR_RAW8: return color_format::raw8;
				case IS_CM_MONO8: return color_format::mono8;
				default: throw std::invalid_argument(std::string("mode"));
				}
			}

			INT to_ueye(color_format mode)
			{
				switch (mode)
				{
				case color_format::raw8: return IS_CM_SENSOR_RAW8;
				case color_format::mono8: return IS_CM_MONO8;
				default: throw std::invalid_argument(std::string("mode"));
				}
			}
		}
	}

	// 
	class color_format_property
	{
	public:
		// 
		color_format_property(HIDS camera_handle) :
			camera_handle(camera_handle)
		{

		}

		// 
		color_format get() const
		{
			INT result = is_SetColorMode(this->camera_handle, IS_GET_COLOR_MODE);
			return detail::convert::from_ueye(result);
		}

		// 
		void set(color_format value) const
		{
			INT result = is_SetColorMode(this->camera_handle, detail::convert::to_ueye(value));
			detail::throw_on_error(this->camera_handle, result);
		}
	private:
		HIDS camera_handle;
	};
}