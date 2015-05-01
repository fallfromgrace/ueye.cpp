#pragma once

#include "uEye.h"

#include "includes.hpp"
#include "error.hpp"

namespace ids
{
	// 
	template<UINT gpio, UINT configuration = IS_GPIO_OUTPUT>
	class gpoi_property
	{
	public:
		gpoi_property(HIDS camera_handle) :
			camera_handle(camera_handle)
		{

		}

		bool_t get() const
		{
			INT result;
			IO_GPIO_CONFIGURATION gpio_configuration;
			gpio_configuration.u32Gpio = gpio;
			gpio_configuration.u32Configuration = configuration;
			result = is_IO(
				this->camera_handle,
				IS_IO_CMD_GPIOS_GET_CONFIGURATION,
				static_cast<void*>(&gpio_configuration),
				sizeof(gpio_configuration));
			detail::throw_on_error(this->camera_handle, result);
			return gpio_configuration.u32State == 1;
		}

		void set(bool_t value) const
		{
			INT result;
			IO_GPIO_CONFIGURATION gpio_configuration;
			gpio_configuration.u32Gpio = gpio;
			gpio_configuration.u32Configuration = configuration;
			gpio_configuration.u32State = value;
			result = is_IO(
				this->camera_handle,
				IS_IO_CMD_GPIOS_SET_CONFIGURATION,
				static_cast<void*>(&gpio_configuration),
				sizeof(gpio_configuration));
			detail::throw_on_error(this->camera_handle, result);
		}

	private:
		const HIDS camera_handle;
	};
}