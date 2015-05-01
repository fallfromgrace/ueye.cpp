#pragma once

#include <thread>

#include "rx.hpp"

#include "uEye.h"

#include "includes.hpp"
#include "logging\logging.hpp"
#include "win32\win32_event.hpp"

#include "camera_event.hpp"
#include "capture_status.hpp"
#include "error.hpp"
#include "locked_frame_buffer.hpp"

#define CAPTURE_STATUS_ENUM_LIST { \
	IS_CAP_STATUS_API_NO_DEST_MEM,\
	IS_CAP_STATUS_API_CONVERSION_FAILED,\
	IS_CAP_STATUS_API_IMAGE_LOCKED,\
	IS_CAP_STATUS_DRV_OUT_OF_BUFFERS,\
	IS_CAP_STATUS_DRV_DEVICE_NOT_READY,\
	IS_CAP_STATUS_USB_TRANSFER_FAILED,\
	IS_CAP_STATUS_DEV_TIMEOUT,\
	IS_CAP_STATUS_ETH_BUFFER_OVERRUN,\
	IS_CAP_STATUS_ETH_MISSED_IMAGES }

namespace ids
{
	namespace detail
	{
		// Translates the specified capture status into a more readable std::string message.
		std::string get_capture_status_message(UEYE_CAPTURE_STATUS capture_status)
		{
			switch (capture_status)
			{
			case IS_CAP_STATUS_API_NO_DEST_MEM: return "No destination memory";
			case IS_CAP_STATUS_API_CONVERSION_FAILED: return "Image conversion failure";
			case IS_CAP_STATUS_API_IMAGE_LOCKED: return "Image locked";
			case IS_CAP_STATUS_DRV_OUT_OF_BUFFERS: return "Out of buffers";
			case IS_CAP_STATUS_DRV_DEVICE_NOT_READY: return "Device not ready";
			case IS_CAP_STATUS_USB_TRANSFER_FAILED: return "USB transfer failed";
			case IS_CAP_STATUS_DEV_TIMEOUT: return "Device timeout";
			case IS_CAP_STATUS_ETH_BUFFER_OVERRUN: return "Buffer overrun";
			case IS_CAP_STATUS_ETH_MISSED_IMAGES: return "Missed images";
			default: throw std::invalid_argument("UEYE_CAPTURE_STATUS value not defined");
			}
		}
	}

	// We are using a class to hold the event loop so that during shut down, the destructor will 
	// wait for the capture loop to end before activated_camera closes the camera, hopefully 
	// preventing any needless errors during shut down.
	// There is potential for deadlock if observer.on_completed() blocks on the thread that 
	// this instance was created on.
	class image_captured_observable
	{
	public:
		// 
		image_captured_observable(HIDS camera_handle)
		{
			win32::manual_reset_event_ptr initialization_completed_event = win32::make_manual_reset_event_ptr();
			this->capture_thread = std::thread(
				&image_captured_observable::when_frame_captured_loop, 
				camera_handle,
				this->destructor_event.handle(),
				this->when_frame_captured_subject.get_subscriber(),
				initialization_completed_event);
			initialization_completed_event->wait_one();
		}

		// 
		image_captured_observable(const image_captured_observable&) = delete;

		// 
		image_captured_observable& operator=(const image_captured_observable&) = delete;
		
		// Waits for capture thread to complete.
		~image_captured_observable()
		{
			this->destructor_event.set();
			if (this->capture_thread.joinable() == true)
				this->capture_thread.join();
		}

		// Returns an observable sequence of all captured images.
		rxcpp::observable<locked_frame_buffer_ptr> get() const
		{
			return this->when_frame_captured_subject.get_observable();
		}
	private:
		typedef rxcpp::subjects::subject<locked_frame_buffer_ptr>::subscriber_type subscriber_type;
		// Loop that monitors all incoming capture events from this camera.
		static void when_frame_captured_loop(
			HIDS camera_handle,
			HANDLE destructor_event_handle,
			const subscriber_type& observer,
			const win32::manual_reset_event_ptr& initialization_completed_event)
		{
			frame_buffer_sequence<10> sequence(camera_handle);
			capture_status_property capture_status(camera_handle);
			frame_captured_event frame_captured_event(camera_handle);
			capture_status_event capture_status_event(camera_handle);

			std::array<HANDLE, 3> event_handles =
			{
				frame_captured_event.handle(),
				capture_status_event.handle(),
				destructor_event_handle
			};

			initialization_completed_event->set();

			{
				::WaitForMultipleObjects(
					event_handles.size(), 
					event_handles.data(), 
					false, 
					INFINITE);
			}

			for (;;)
			{
				// We get the active sequence buffer first before waiting instead of waiting then 
				// retrieving the last captured sequence buffer because the UEYE API does not seem 
				// to cleanly support that concept.  In any case, the last captured buffer is 
				// potentially locked, creating a potential bug, whereas the active buffer implies 
				// that cannot be currently locked.
				frame_buffer_info active_frame_buffer_info = sequence.get_active_buffer().info();
				DWORD result = ::WaitForMultipleObjects(
					event_handles.size(), 
					event_handles.data(), 
					false, 
					INFINITE);

				switch (result)
				{
				case WAIT_OBJECT_0 + 0:
					observer.on_next(make_locked_frame_buffer_ptr(
						camera_handle,
						active_frame_buffer_info));
					break;
				case WAIT_OBJECT_0 + 1:
					try
					{
						UEYE_CAPTURE_STATUS_INFO capture_status_info = capture_status.get();
						if (capture_status_info.dwCapStatusCnt_Total > 0)
							for (auto capture_status : CAPTURE_STATUS_ENUM_LIST)
								if (capture_status_info.adwCapStatusCnt_Detail[capture_status] > 0)
									log_warn(
										"Capture error in camera %i: %s, Count: %i",
										camera_handle,
										detail::get_capture_status_message(capture_status).c_str(),
										capture_status_info.adwCapStatusCnt_Detail[capture_status]);
						capture_status.reset();
					}
					catch (const ids_exception& ex)
					{
						log_error(
							"Error getting capture status: %s",
							ex.what());
					}
					break;
				case WAIT_OBJECT_0 + 2:
					observer.on_completed();
					return;
				case WAIT_ABANDONED_0 + 0:
					log_error(
						"WaitForMultipleObjects() failed in when_frame_captured_loop() with code %i: %s",
						WAIT_ABANDONED_0,
						"Wait handle for frame_captured_event abandoned");
					observer.on_completed();
					return;
				case WAIT_ABANDONED_0 + 1:
					log_error(
						"WaitForMultipleObjects() failed in when_frame_captured_loop() with code %i: %s",
						WAIT_ABANDONED_0,
						"Wait handle for capture_status_event abandoned");
					observer.on_completed();
					return;
				case WAIT_ABANDONED_0 + 2:
					log_error(
						"WaitForMultipleObjects() failed in when_frame_captured_loop() with code %i: %s",
						WAIT_ABANDONED_0,
						"Wait handle for destructor_event abandoned");
					observer.on_completed();
					return;
				case WAIT_TIMEOUT:
					log_error(
						"WaitForMultipleObjects() failed in when_frame_captured_loop() with code %i: %s",
						WAIT_TIMEOUT,
						"Wait timed out");
					observer.on_completed();
					return;
				case WAIT_FAILED:
					log_error(
						"WaitForMultipleObjects() failed in when_frame_captured_loop() with code %i: %s",
						WAIT_FAILED,
						"Wait failed, %s",
						win32::get_last_win32_error_message().c_str());
					observer.on_error(std::make_exception_ptr(win32::get_last_win32_exception()));
					return;
				default:
					log_error(
						"WaitForMultipleObjects() failed in when_frame_captured_loop() with unknown code %i: %s",
						result,
						win32::get_last_win32_error_message().c_str());
					observer.on_error(std::make_exception_ptr(win32::get_last_win32_exception()));
					return;
				}
			}
		}

		std::thread capture_thread;
		win32::manual_reset_event destructor_event;
		rxcpp::subjects::subject<locked_frame_buffer_ptr> when_frame_captured_subject;
	};
}