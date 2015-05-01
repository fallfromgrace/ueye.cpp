#pragma once

#include "uEye.h"

#include "includes.hpp"
#include "error.hpp"
#include "logging\logging.hpp"

#include "frame_buffer_info.hpp"

namespace ids
{
	namespace detail
	{
		// 
		template<typename data_type>
		class dynamic_array
		{
		public:

			dynamic_array(size_t n) :
				p(new data_type[n])
			{

			}

			dynamic_array(dynamic_array&& other) : 
				p(other.p)
			{
				other.p = nullptr;
			}

			~dynamic_array()
			{
				delete[] p;
				p = nullptr;
			}

			data_type* data() const
			{
				return p;
			}

		private:
			data_type* p;
		};
	}

	// Manages the resources of a frame buffer.
	class frame_buffer
	{
	public:
		// 
		frame_buffer(
			HIDS camera_handle,
			int32_t width,
			int32_t height,
			int32_t bits_per_pixel) :
			camera_handle(camera_handle),
			buffer(width * height * (bits_per_pixel + 1) / 8)
		{
			INT result = is_SetAllocatedImageMem(
				this->camera_handle,
				static_cast<INT>(width),
				static_cast<INT>(height),
				static_cast<INT>(bits_per_pixel),
				this->buffer.data(),
				&this->buffer_id);
			detail::throw_on_error(this->camera_handle, result);
		}

		// 
		frame_buffer(frame_buffer&& other) :
			camera_handle(other.camera_handle),
			buffer_id(other.buffer_id),
			buffer(std::move(other.buffer))
		{
			other.camera_handle = 0;
			other.buffer_id = 0;
		}

		// 
		frame_buffer(const frame_buffer& other) = delete;

		// 
		frame_buffer& operator=(const frame_buffer& other) = delete;

		// 
		~frame_buffer()
		{
			if (this->camera_handle != 0)
			{
				INT result = is_FreeImageMem(
					this->camera_handle,
					this->buffer.data(),
					this->buffer_id);
				if (result != IS_SUCCESS)
					log_error(
						"is_FreeImageMem(%i, 0x%08x, %i) failed in ~frame_buffer() with code %i: %s",
						this->camera_handle,
						this->buffer.data(),
						this->buffer_id,
						result,
						detail::get_error_message(this->camera_handle).c_str());
			}
		}

		// 
		frame_buffer_info info() const
		{
			return frame_buffer_info(this->buffer_id, this->buffer.data());
		}
	private:
		detail::dynamic_array<char> buffer;
		INT buffer_id;
		HIDS camera_handle;
	};
}