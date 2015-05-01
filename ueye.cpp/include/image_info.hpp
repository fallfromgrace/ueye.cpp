#pragma once

#include <chrono>
#include <ctime>

#include "uEye.h"
#include "includes.hpp"

#include "error.hpp"

namespace ids
{
	namespace detail
	{
		// 
		std::chrono::system_clock::time_point convert_to_time_point(const UEYETIME& time)
		{
			std::tm tm = { 0 };
			tm.tm_sec = static_cast<int>(time.wSecond);
			tm.tm_min = static_cast<int>(time.wMinute);
			tm.tm_hour = static_cast<int>(time.wHour);
			tm.tm_mday = static_cast<int>(time.wDay);
			tm.tm_mon = static_cast<int>(time.wMonth - 1);
			tm.tm_year = static_cast<int>(time.wYear - 1900);
			tm.tm_isdst = -1;

			std::time_t tt = ::mktime(&tm);
			auto tp = std::chrono::system_clock::from_time_t(tt);
			tp += std::chrono::milliseconds(time.wMilliseconds);

			return tp;
		}
	}

	// 
	class image_info
	{
	public:
		// 
		image_info(
			HIDS camera_handle,
			INT buffer_id,
			char* buffer_data) :
			camera_handle(camera_handle),
			buffer_data(buffer_data)
		{
			INT result = is_GetImageInfo(
				this->camera_handle,
				buffer_id,
				&this->ueye_image_info,
				sizeof(this->ueye_image_info));
			detail::throw_on_error(this->camera_handle, result);
		}

		// 
		std::chrono::system_clock::time_point timestamp() const
		{
			return detail::convert_to_time_point(this->ueye_image_info.TimestampSystem);
		}

		// 
		int32_t width() const
		{
			return static_cast<int32_t>(this->ueye_image_info.dwImageWidth);
		}

		// 
		int32_t height() const
		{
			return static_cast<int32_t>(this->ueye_image_info.dwImageHeight);
		}

		// 
		int32_t stride() const
		{
			INT pixel_pitch, result;
			result = is_GetImageMemPitch(this->camera_handle, &pixel_pitch);
			detail::throw_on_error(this->camera_handle, result);
			return static_cast<int32_t>(pixel_pitch);
		}

		// 
		uint8_t* data() const
		{
			return reinterpret_cast<uint8_t*>(this->buffer_data);
		}
	private:
		const HIDS camera_handle;
		char* const buffer_data;
		UEYEIMAGEINFO ueye_image_info;
	};
}