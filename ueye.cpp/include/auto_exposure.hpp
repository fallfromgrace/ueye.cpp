#pragma once

#include "uEye.h"

#include "error.hpp"
#include "includes.hpp"

namespace ids
{
	// 
	class auto_exposure_reference_property
	{
	public:
		//
		auto_exposure_reference_property(HIDS camera_handle) :
			camera_handle(camera_handle)
		{

		}

		// Gets the reference auto exposure.
		int32_t get() const
		{
			return this->get<IS_GET_AUTO_REFERENCE>();
		}

		// Sets the reference auto exposure.
		void set(int32_t value) const
		{
			double reference = static_cast<double>(value);
			INT result = is_SetAutoParameter(
				this->camera_handle,
				IS_SET_AUTO_REFERENCE,
				&reference,
				nullptr);
			detail::throw_on_error(this->camera_handle, result);
		}
	private:

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

		const HIDS camera_handle;
	};

	// 
	class auto_exposure_property
	{
	public:
		//
		auto_exposure_property(HIDS camera_handle) :
			camera_handle(camera_handle)
		{

		}

		// Gets if auto exposure is enabled.
		bool_t get() const
		{
			double enable;
			INT result = is_SetAutoParameter(
				this->camera_handle, 
				IS_GET_ENABLE_AUTO_SHUTTER, 
				&enable, 
				nullptr);
			detail::throw_on_error(this->camera_handle, result);
			return enable == 1;
		}

		// Sets if the auto exposure is enabled.
		void set(bool_t value) const
		{
			double enable = value;
			INT result = is_SetAutoParameter(
				this->camera_handle, 
				IS_SET_ENABLE_AUTO_SHUTTER, 
				&enable, 
				nullptr);
			detail::throw_on_error(this->camera_handle, result);
		}

		auto_exposure_reference_property reference() const
		{
			return auto_exposure_reference_property(this->camera_handle);
		}

	private:
		const HIDS camera_handle;
	};
}