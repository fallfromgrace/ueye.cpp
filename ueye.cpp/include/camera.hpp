#pragma once

#ifndef UEYE_CPP_CAMERA_HPP
#define UEYE_CPP_CAMERA_HPP

#include <memory>
#include <string>
#include <Windows.h>

#include "rx.hpp"
#include "uEye.h"

#include "error.hpp"
#include "includes.hpp"
#include "logging\logging.hpp"

#include "auto_exposure.hpp"
#include "capture.hpp"
#include "color_format.hpp"
#include "exposure.hpp"
#include "flash.hpp"
#include "framerate.hpp"
#include "gpio.hpp"
#include "pixel_clock.hpp"
#include "roi.hpp"
#include "standby.hpp"
#include "trigger.hpp"

#include "camera_event.hpp"
#include "frame_buffer_sequence.hpp"
#include "image_captured_observable.hpp"
#include "locked_frame_buffer.hpp"

namespace ids
{
	namespace detail
	{
		// Controls communication with the IDS driver for a particular camera.
		class activated_camera
		{
		public:
			// activates the specified camera from the UEYE_CAMERA_INFO struct.
			// Optionally, takes a window handle in order to enable 
			// USB connect/disconnect events.
			activated_camera(
				const UEYE_CAMERA_INFO& camera_info,
				HWND window_handle = nullptr) :
				camera_handle(camera_info.dwCameraID),
				camera_id(camera_info.dwCameraID),
				model_number(camera_info.Model),
				name(camera_info.FullModelName),
				serial_number(camera_info.SerNo)
			{
				INT result = is_InitCamera(&this->camera_handle, window_handle);
				detail::throw_on_error(this->camera_handle, result);
				
				this->when_image_captured_observable = 
					std::make_unique<image_captured_observable>(this->camera_handle);
			}

			//
			activated_camera(const activated_camera&) = delete;

			//
			activated_camera& operator=(const activated_camera&) = delete;

			//
			~activated_camera()
			{
				INT result = is_ExitCamera(this->camera_handle);
				if (result != IS_SUCCESS)
					log_error(
						"is_ExitCamera(%i) failed in ~activated_camera() with code %i: %s", 
						this->camera_handle, 
						result, 
						detail::get_error_message(this->camera_handle));
			}

			// Gets the id, which is a programmable unique identifier
			DWORD id() const
			{
				return this->camera_id;
			}

			// Gets the auto exposure settings.
			auto_exposure_property auto_exposure() const
			{
				return auto_exposure_property(this->camera_handle);
			}

			// Gets the capture settings
			capture_property capture() const
			{
				return capture_property(this->camera_handle);
			}

			// Gets the color format settings
			color_format_property color_format() const
			{
				return color_format_property(this->camera_handle);
			}

			// Gets the exposure settings.
			exposure_property exposure() const
			{
				return exposure_property(this->camera_handle);
			}

			flash_property flash() const
			{
				return flash_property(this->camera_handle);
			}

			// TODO alter impl based on trigger mode.
			framerate_property framerate() const
			{
				return framerate_property(this->camera_handle);
			}

			// Gets the gpio settings
			template<UINT gpio>
			gpoi_property<gpio> gpio() const
			{
				return gpoi_property<gpio>(this->camera_handle);
			}

			// Gets the pixel clock settings.
			pixel_clock_property pixel_clock() const
			{
				return pixel_clock_property(this->camera_handle);
			}

			// Resets all camera settings to their default values.
			void reset() const
			{
				INT result = is_ResetToDefault(this->camera_handle);
				detail::throw_on_error(this->camera_handle, result);
			}

			// 
			standby_property standby() const
			{
				return standby_property(this->camera_handle);
			}

			// Gets the external trigger settings.
			trigger_property trigger() const
			{
				return trigger_property(this->camera_handle);
			}

			// 
			rxcpp::observable<locked_frame_buffer_ptr> when_image_captured() const
			{
				return this->when_image_captured_observable->get();
			}
		private:
			HIDS camera_handle;
			DWORD camera_id;
			const std::string model_number;
			const std::string name;
			const std::string serial_number;
			std::unique_ptr<image_captured_observable> when_image_captured_observable;
		};
	}

	// 
	typedef std::unique_ptr<detail::activated_camera> camera_ptr;

	// 
	typedef camera_ptr::pointer camera_pointer;

	//
	camera_ptr activate_camera(const UEYE_CAMERA_INFO& camera_info)
	{
		return std::make_unique<detail::activated_camera>(camera_info);
	}
}

#endif//UEYE_CPP_CAMERA_HPP