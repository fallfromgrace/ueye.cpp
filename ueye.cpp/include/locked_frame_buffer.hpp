#pragma once

#include <memory>

#include "uEye.h"

#include "includes.hpp"
#include "logging\logging.hpp"

#include "error.hpp"
#include "image_info.hpp"
#include "frame_buffer_info.hpp"

namespace ids
{
	namespace detail
	{
		// Acquires a lock on the specified frame buffer.  
		// Releases the lock upon destruction.
		class locked_frame_buffer
		{
		public:
			// 
			locked_frame_buffer(
				HIDS camera_handle,
				const frame_buffer_info& frame_buffer_info) :
				camera_handle(camera_handle),
				frame_buffer_info(frame_buffer_info)
			{
				//INT result = is_LockSeqBuf(
				//	this->camera_handle,
				//	this->frame_buffer_info.id(),
				//	this->frame_buffer_info.data());
				//detail::throw_on_error(this->camera_handle, result);
			}

			// 
			locked_frame_buffer(const locked_frame_buffer&) = delete;

			// 
			locked_frame_buffer& operator=(const locked_frame_buffer&) = delete;

			// 
			locked_frame_buffer(locked_frame_buffer&& other) :
				camera_handle(other.camera_handle),
				frame_buffer_info(std::move(other.frame_buffer_info))
			{
				other.camera_handle = 0;
			}

			// 
			locked_frame_buffer& operator=(locked_frame_buffer&& other)
			{
				if (this->camera_handle != 0)
				{
					INT result = is_UnlockSeqBuf(
						this->camera_handle,
						this->frame_buffer_info.id(),
						this->frame_buffer_info.data());
					detail::throw_on_error(this->camera_handle, result);
				}

				this->camera_handle = other.camera_handle;
				this->frame_buffer_info = std::move(other.frame_buffer_info);
				other.camera_handle = 0;
				return *this;
			}

			// Releases lock on the sequence buffer.
			~locked_frame_buffer()
			{
				if (this->camera_handle != 0)
				{
					INT result = is_UnlockSeqBuf(
						this->camera_handle,
						this->frame_buffer_info.id(),
						this->frame_buffer_info.data());
					if (result != IS_SUCCESS)
						log_error(
							"is_UnlockSeqBuf(%i, %i, 0x%08x) failed in ~locked_sequence_buffer() with code %i: %s",
							this->camera_handle,
							this->frame_buffer_info.id(),
							this->frame_buffer_info.data(),
							result,
							detail::get_error_message(this->camera_handle).c_str());
				}
			}

			// Gets the sequence buffer's image info, including timestamp.
			image_info get_image() const
			{
				return image_info(
					this->camera_handle,
					this->frame_buffer_info.id(),
					this->frame_buffer_info.data());
			}
		private:
			HIDS camera_handle;
			frame_buffer_info frame_buffer_info;
		};
	}

	// Normally would use a single ownership model with unique_ptr or simply std::move, but rx does 
	// not support move-only types.
	typedef std::shared_ptr<detail::locked_frame_buffer> locked_frame_buffer_ptr;

	// Creates a new ref-counted locked frame buffer, which ensures frames are locked for the 
	// while in use.  
	locked_frame_buffer_ptr make_locked_frame_buffer_ptr(
		HIDS camera_handle,
		frame_buffer_info frame_buffer_info)
	{
		return std::make_shared<detail::locked_frame_buffer>(camera_handle, frame_buffer_info);
	}
}