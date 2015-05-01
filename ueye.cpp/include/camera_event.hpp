#pragma once

#include <Windows.h>

#include "includes.hpp"
#include "logging\logging.hpp"
#include "uEye.h"
#include "win32\win32_event.hpp"

#include "error.hpp"

namespace ids
{
	namespace detail
	{
		// 
		template<INT event_type>
		class camera_event : public win32::auto_reset_event
		{
		public:
			// 
			camera_event(HIDS camera_handle) :
				win32::auto_reset_event(),
				camera_handle(camera_handle)
			{
				INT result;

				result = is_InitEvent(this->camera_handle, this->handle(), event_type);
				throw_on_error(this->camera_handle, result);

				result = is_EnableEvent(this->camera_handle, event_type);
				throw_on_error(this->camera_handle, result);
			}

			// 
			camera_event(const camera_event&) = delete;

			// 
			camera_event& operator=(const camera_event&) = delete;

			// 
			~camera_event()
			{
				INT result;

				result = is_DisableEvent(this->camera_handle, event_type);
				if (result != IS_SUCCESS)
					log_error(
						"is_DisableEvent(%i) failed in ~camera_event() with code %i: %s",
						this->camera_handle,
						result,
						detail::get_error_message(this->camera_handle).c_str());

				result = is_ExitEvent(this->camera_handle, event_type);
				if (result != IS_SUCCESS)
					log_error(
						"is_ExitEvent(%i) failed in ~camera_event() with code %i: %s",
						this->camera_handle,
						result,
						detail::get_error_message(this->camera_handle).c_str());
			}

			// Gets the underlying Win32 event handle.
			const HANDLE get() const
			{
				return this->event_handle;
			}
		private:
			const HIDS camera_handle;
		};
	}

	// 
	typedef detail::camera_event<IS_SET_EVENT_FRAME> frame_captured_event;

	// 
	typedef detail::camera_event<IS_SET_EVENT_CAPTURE_STATUS> capture_status_event;

	// Requires win32 message loop.
	typedef detail::camera_event<IS_SET_EVENT_REMOVAL> camera_removed_event;

	// Requires win32 message loop.
	typedef detail::camera_event<IS_SET_EVENT_DEVICE_RECONNECTED> camera_reconnected_event;
}