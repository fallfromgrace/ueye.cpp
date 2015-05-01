#pragma once

#include "uEye.h"

#include "includes.hpp"
#include "error.hpp"

namespace ids
{
	// 
	enum flash_mode
	{
		trigger_hi,
		trigger_lo,
		constant_hi,
		constant_lo,
		freerun_hi,
		freerun_lo,
		off
	};

	namespace detail
	{
		// 
		flash_mode convert_to_flash_mode(INT flash_mode)
		{
			switch (flash_mode)
			{
			case IO_FLASH_MODE_TRIGGER_LO_ACTIVE: return flash_mode::trigger_lo;
			default: throw std::invalid_argument("Invalid argument.");
			}
		}

		// 
		INT convert_to_ids(flash_mode flash_mode)
		{
			switch (flash_mode)
			{
			case flash_mode::trigger_lo: return IO_FLASH_MODE_TRIGGER_LO_ACTIVE;
			default: throw std::invalid_argument("Invalid argument.");
			}
		}

		// 
		bool_t convert_to_bool_t(INT auto_flash)
		{
			switch (auto_flash)
			{
			case IS_FLASH_AUTO_FREERUN_ON : return true;
			case IS_FLASH_AUTO_FREERUN_OFF: return false;
			default: throw std::invalid_argument("Invalid argument.");
			}
		}

		// 
		INT convert_to_ids(bool_t auto_flash)
		{
			if (auto_flash == true)
				return IS_FLASH_AUTO_FREERUN_ON;
			else
				return IS_FLASH_AUTO_FREERUN_OFF;
		}
	}

	// 
	class flash_mode_property
	{
	public:
		//
		flash_mode_property(HIDS camera_handle) :
			camera_handle(camera_handle)
		{

		}

		// 
		flash_mode get() const
		{
			INT flash_mode;
			INT result = is_IO(
				this->camera_handle,
				IS_IO_CMD_FLASH_GET_MODE,
				&flash_mode,
				sizeof(flash_mode));
			detail::throw_on_error(this->camera_handle, result);
			return detail::convert_to_flash_mode(flash_mode);
		}

		// 
		void set(flash_mode value) const
		{
			INT flash_mode = detail::convert_to_ids(value);
			INT result = is_IO(
				this->camera_handle,
				IS_IO_CMD_FLASH_SET_MODE,
				&flash_mode,
				sizeof(flash_mode));
			detail::throw_on_error(this->camera_handle, result);
		}

	private:
		const HIDS camera_handle;
	};

	// 
	class auto_flash_property
	{
	public:
		// 
		auto_flash_property(HIDS camera_handle) :
			camera_handle(camera_handle)
		{

		}

		// Gets the default auto flash mode.
		bool_t default() const
		{
			INT auto_flash;
			INT result = is_IO(
				this->camera_handle,
				IS_IO_CMD_FLASH_GET_AUTO_FREERUN_DEFAULT,
				&auto_flash,
				sizeof(auto_flash));
			detail::throw_on_error(this->camera_handle, result);
			return detail::convert_to_bool_t(auto_flash);
		}

		// Gets if the auto flash is enabled in freerun mode.
		bool_t get() const
		{
			INT auto_flash;
			INT result = is_IO(
				this->camera_handle,
				IS_IO_CMD_FLASH_GET_AUTO_FREERUN,
				&auto_flash,
				sizeof(auto_flash));
			detail::throw_on_error(this->camera_handle, result);
			return detail::convert_to_bool_t(auto_flash);
		}

		// Sets if the auto flash is enabled in freerun mode.
		void set(bool_t value) const
		{
			INT auto_flash = detail::convert_to_ids(value);
			INT result = is_IO(
				this->camera_handle,
				IS_IO_CMD_FLASH_SET_AUTO_FREERUN,
				&auto_flash,
				sizeof(auto_flash));
			detail::throw_on_error(this->camera_handle, result);
		}

	private:
		const HIDS camera_handle;
	};

	// 
	class flash_parameters_property
	{
	public:
		// 
		flash_parameters_property(HIDS camera_handle) :
			camera_handle(camera_handle)
		{

		}

		// Gets the minimum flash parameters
		IO_FLASH_PARAMS min() const
		{
			IO_FLASH_PARAMS flash_parameters;
			INT result = is_IO(
				this->camera_handle,
				IS_IO_CMD_FLASH_GET_PARAMS_MIN,
				&flash_parameters,
				sizeof(flash_parameters));
			detail::throw_on_error(this->camera_handle, result);
			return flash_parameters;
		}

		// Gets the maximum flash parameters
		IO_FLASH_PARAMS max() const
		{
			IO_FLASH_PARAMS flash_parameters;
			INT result = is_IO(
				this->camera_handle,
				IS_IO_CMD_FLASH_GET_PARAMS_MAX,
				&flash_parameters,
				sizeof(flash_parameters));
			detail::throw_on_error(this->camera_handle, result);
			return flash_parameters;
		}

		// Gets the increment flash parameters
		IO_FLASH_PARAMS inc() const
		{
			IO_FLASH_PARAMS flash_parameters;
			INT result = is_IO(
				this->camera_handle,
				IS_IO_CMD_FLASH_GET_PARAMS_INC,
				&flash_parameters,
				sizeof(flash_parameters));
			detail::throw_on_error(this->camera_handle, result);
			return flash_parameters;
		}

		// Gets the current flash parameters.
		IO_FLASH_PARAMS get() const
		{
			IO_FLASH_PARAMS flash_parameters;
			INT result = is_IO(
				this->camera_handle,
				IS_IO_CMD_FLASH_GET_PARAMS,
				&flash_parameters,
				sizeof(flash_parameters));
			detail::throw_on_error(this->camera_handle, result);
			return flash_parameters;
		}

		// Sets the current flash paramaters.
		void set(IO_FLASH_PARAMS value) const
		{
			INT result = is_IO(
				this->camera_handle,
				IS_IO_CMD_FLASH_SET_PARAMS,
				&value,
				sizeof(value));
			detail::throw_on_error(this->camera_handle, result);
		}
	private:
		const HIDS camera_handle;
	};

	// 
	class flash_duration_property
	{
	public:

		flash_duration_property(HIDS camera_handle) :
			flash_parameters_property(camera_handle)
		{

		}

		// 
		int32_t min() const
		{
			return static_cast<int32_t>(this->flash_parameters_property.min().u32Duration);
		}

		// 
		int32_t max() const
		{
			return static_cast<int32_t>(this->flash_parameters_property.max().u32Duration);
		}

		// 
		int32_t inc() const
		{
			return static_cast<int32_t>(this->flash_parameters_property.inc().u32Duration);
		}

		// 
		int32_t get() const
		{
			return static_cast<int32_t>(this->flash_parameters_property.get().u32Duration);
		}

		// 
		void set(int32_t value) const
		{
			IO_FLASH_PARAMS flash_parameters = this->flash_parameters_property.get();
			flash_parameters.u32Duration = static_cast<UINT>(value);
			this->flash_parameters_property.set(flash_parameters);
		}
	private:
		const flash_parameters_property flash_parameters_property;
	};

	// 
	class flash_delay_property
	{
	public:
		// 
		flash_delay_property(HIDS camera_handle) :
			flash_parameters_property(camera_handle)
		{

		}

		// 
		int32_t min() const
		{
			return static_cast<int32_t>(this->flash_parameters_property.min().s32Delay);
		}

		// 
		int32_t max() const
		{
			return static_cast<int32_t>(this->flash_parameters_property.max().s32Delay);
		}

		// 
		int32_t inc() const
		{
			return static_cast<int32_t>(this->flash_parameters_property.inc().s32Delay);
		}

		// 
		int32_t get() const
		{
			return static_cast<int32_t>(this->flash_parameters_property.get().s32Delay);
		}

		// 
		void set(int32_t value) const
		{
			IO_FLASH_PARAMS flash_parameters = this->flash_parameters_property.get();
			flash_parameters.s32Delay = static_cast<UINT>(value);
			this->flash_parameters_property.set(flash_parameters);
		}
	private:
		const flash_parameters_property flash_parameters_property;
	};

	// 
	class flash_property
	{
	public:
		// 
		flash_property(HIDS camera_handle) :
			camera_handle(camera_handle)
		{

		}

		// 
		flash_mode_property mode() const
		{
			return flash_mode_property(this->camera_handle);
		}

		// 
		auto_flash_property auto_flash() const
		{
			return auto_flash_property(this->camera_handle);
		}

		// 
		flash_delay_property delay() const
		{
			return flash_delay_property(this->camera_handle);
		}

		// 
		flash_duration_property duration() const
		{
			return flash_duration_property(this->camera_handle);
		}
	private:
		const HIDS camera_handle;
	};
}