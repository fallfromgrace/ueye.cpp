#pragma once

#include "uEye.h"

#include "includes.hpp"
#include "error.hpp"

namespace ueye
{
	namespace detail
	{
		// 
		class camera_property
		{
		public:
			// 
			camera_property(HIDS camera_handle) :
				camera_handle(camera_handle)
			{

			}
		protected:
			// 
			HIDS handle() const
			{
				return this->camera_handle;
			}
		private:
			HIDS camera_handle;
		};
	}
}