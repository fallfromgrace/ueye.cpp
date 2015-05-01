#pragma once

#include "includes.hpp"
#include "uEye.h"

namespace ids
{
	namespace detail
	{
		template<UINT command>
		struct command_traits;

		struct IS_RECT_type
		{
			typedef IS_RECT value_type;
		};

		struct IS_POINT_2D_type
		{
			typedef IS_POINT_2D value_type;
		};

		template<>
		struct command_traits<IS_AOI_IMAGE_GET_AOI> : public IS_RECT_type
		{

		};

		template<>
		struct command_traits<IS_AOI_IMAGE_SET_AOI> : public IS_RECT_type
		{

		};
	}

	// 
	class roi_property
	{
	public:
		//
		roi_property(HIDS camera_handle) :
			camera_handle(camera_handle)
		{

		}

		// 
		IS_RECT get() const
		{
			return get<IS_AOI_IMAGE_GET_AOI>();
		}

		// 
		void set(IS_RECT value) const
		{
			set<IS_AOI_IMAGE_SET_AOI>(value);
		}
	private:
		// 
		template<UINT command, typename value_type = detail::command_traits<command>::value_type>
		value_type get() const
		{
			value_type value;
			INT result = is_AOI(
				this->camera_handle,
				command, 
				&value, 
				sizeof(value));
			detail::throw_on_error(this->camera_handle, result);
		}

		// 
		template<UINT command, typename value_type = detail::command_traits<command>::value_type>
		void set(value_type value) const
		{
			INT result = is_AOI(
				this->camera_handle,
				command,
				&value,
				sizeof(value));
			detail::throw_on_error(this->camera_handle, result);
		}

		const HIDS camera_handle;
	};
}