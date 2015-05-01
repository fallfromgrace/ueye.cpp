#pragma once

#include "uEye.h"

#include "includes.hpp"
//#include "include\error.hpp"

#include "camera_property.hpp"

namespace ueye
{
	namespace detail
	{
		// 
		class auto_parameter : public camera_property
		{
		public:
			// 
			auto_parameter(HIDS camera_handle) :
				camera_property(camera_handle)
			{

			}

		protected:
			template<INT param>
			int32_t get() const
			{
				double reference;
				INT result = is_SetAutoParameter(
					this->camera_handle,
					param,
					&reference,
					nullptr);
				detail::throw_on_error(this->camera_handle, result);
				return static_cast<int32_t>(reference);
			}

			template<INT param>
			void set(int32_t value) const
			{
				double fvalue = static_cast<double>(value);
				INT result = is_SetAutoParameter(
					this->camera_handle,
					param,
					&fvalue,
					nullptr);
				detail::throw_on_error(this->camera_handle, result);
			}
		};
	}
}