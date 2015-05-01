#pragma once

#include <memory>

#include "rx.hpp"

#include "uEye.h"

#include "includes.hpp"
#include "error.hpp"
#include "camera_event.hpp"
#include "locked_frame.hpp"
#include "sequence_buffer.hpp"

namespace ids
{

	class when_image_captured_on_subscribe
	{
	public:
		// 
		when_image_captured_on_subscribe(
			HIDS camera_handle,
			const manual_reset_event& cancellation_event) :
			camera_handle(camera_handle),
			frame_captured_event(camera_handle)
		{

		}

		// 
		template<typename subscriber>
		void operator ()(subscriber&& subcriber)
		{
			for (;;)
			{
				sequence_buffer active_sequence_buffer = get_active_sequence_buffer(this->camera_handle);
				DWORD result = ::WaitForMultipleObjects(2, event_handles.data(), false, INFINITE);

				switch (result)
				{
				case WAIT_OBJECT_0 + 0:
					o.on_next(active_sequence_buffer);
					break;
				case WAIT_OBJECT_0 + 1:
					o.on_completed();
					return;
				case WAIT_ABANDONED_0 + 0:
					printf(
						"WaitForMultipleObjects() failed in ~camera_event() with code %i: \"%s\"\n",
						WAIT_ABANDONED_0,
						"WAIT_ABANDONED_0");
					//o.on_error(std::make_exception_ptr(win32_exception()));
					o.on_completed();
					return;
				case WAIT_ABANDONED_0 + 1:
					printf(
						"WaitForMultipleObjects() failed in ~camera_event() with code %i: \"%s\"\n",
						WAIT_ABANDONED_0,
						"WAIT_ABANDONED_0");
					//o.on_error(std::make_exception_ptr(win32_exception()));
					o.on_completed();
					return;
				case WAIT_TIMEOUT:
					printf(
						"WaitForMultipleObjects() failed in ~camera_event() with code %i: \"%s\"\n",
						WAIT_TIMEOUT,
						"WAIT_TIMEOUT");
					//o.on_error(std::make_exception_ptr(win32_exception()));
					o.on_completed();
					return;
				case WAIT_FAILED:
					printf(
						"WaitForMultipleObjects() failed in ~camera_event() with code %i: \"%s\"\n",
						WAIT_FAILED,
						"WAIT_FAILED");
				default:
					//o.on_error(std::make_exception_ptr(win32_exception()));
					o.on_completed();
					return;
				}
			}
		}
	private:
		HIDS camera_handle;
		manual_reset_event cancellation_event;
		frame_captured_event frame_captured_event;
	};

	rxcpp::observable<sequence_buffer> create_when_image_captured_observable(HIDS camera_handle)
	{
		//rxcpp::make_subscription()

		return rxcpp::sources::create<sequence_buffer>([&](rxcpp::subscriber<int> o)
		{
			manual_reset_event cancellation_event;
			frame_captured_event frame_captured_event(camera_handle);

			std::array<HANDLE, 2> event_handles =
			{
				frame_captured_event.handle(),
				cancellation_event.handle()
			};

			for (;;)
			{
				sequence_buffer active_sequence_buffer = get_active_sequence_buffer(camera_handle);
				DWORD result = ::WaitForMultipleObjects(2, event_handles.data(), false, INFINITE);

				switch (result)
				{
				case WAIT_OBJECT_0 + 0:
					o.on_next(active_sequence_buffer);
					break;
				case WAIT_OBJECT_0 + 1:
					o.on_completed();
					return;
				case WAIT_ABANDONED_0 + 0:
					printf(
						"WaitForMultipleObjects() failed in ~camera_event() with code %i: \"%s\"\n",
						WAIT_ABANDONED_0,
						"WAIT_ABANDONED_0");
					//o.on_error(std::make_exception_ptr(win32_exception()));
					o.on_completed();
					return;
				case WAIT_ABANDONED_0 + 1:
					printf(
						"WaitForMultipleObjects() failed in ~camera_event() with code %i: \"%s\"\n",
						WAIT_ABANDONED_0,
						"WAIT_ABANDONED_0");
					//o.on_error(std::make_exception_ptr(win32_exception()));
					o.on_completed();
					return;
				case WAIT_TIMEOUT:
					printf(
						"WaitForMultipleObjects() failed in ~camera_event() with code %i: \"%s\"\n",
						WAIT_TIMEOUT,
						"WAIT_TIMEOUT");
					//o.on_error(std::make_exception_ptr(win32_exception()));
					o.on_completed();
					return;
				case WAIT_FAILED:
					printf(
						"WaitForMultipleObjects() failed in ~camera_event() with code %i: \"%s\"\n",
						WAIT_FAILED,
						"WAIT_FAILED");
				default:
					//o.on_error(std::make_exception_ptr(win32_exception()));
					o.on_completed();
					return;
				}
			}

			return rxcpp::make_subscription([&](){ cancellation_event.set(); });
		});
	}
}