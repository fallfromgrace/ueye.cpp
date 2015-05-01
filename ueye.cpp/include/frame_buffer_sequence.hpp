#pragma once

#include <vector>

#include "includes.hpp"
#include "logging\logging.hpp"
#include "uEye.h"

#include "error.hpp"
#include "frame_buffer.hpp"

namespace ids
{
	// Manages the resources for a sequence of frame buffers for the specified camera.
	template<size_t frame_buffer_count>
	class frame_buffer_sequence
	{
	public:
		// 
		frame_buffer_sequence(HIDS camera_handle) :
			camera_handle(camera_handle)
		{
			INT result = is_InitImageQueue(this->camera_handle, 0);
			detail::throw_on_error(this->camera_handle, result);

			for (size_t i = 0; i < frame_buffer_count; ++i)
			{
				this->sequence.emplace_back(
					this->camera_handle, 
					1600,
					1200, 
					8);
				result = is_AddToSequence(
					this->camera_handle,
					this->sequence[i].info().data(),
					this->sequence[i].info().id());
				detail::throw_on_error(this->camera_handle, result);
			}
		}

		// 
		frame_buffer_sequence(frame_buffer_sequence& other) = delete;

		// 
		frame_buffer_sequence& operator=(const frame_buffer_sequence& other) = delete;

		// Releases all allocated memory and removes references to the IDS driver.
		~frame_buffer_sequence()
		{
			INT result;

			result = is_ClearSequence(this->camera_handle);
			if (result != IS_SUCCESS)
				log_error(
					"is_ClearSequence(%i) failed in ~frame_buffer_sequence() with code %i: %s",
					this->camera_handle,
					result,
					detail::get_error_message(this->camera_handle).c_str());

			result = is_ExitImageQueue(this->camera_handle);
			if (result != IS_SUCCESS)
				log_error(
					"is_ExitImageQueue(%i) failed in ~frame_buffer_sequence() with code %i: %s",
					this->camera_handle,
					result,
					detail::get_error_message(this->camera_handle).c_str());
		}

		// Gets the currently active frame buffer.
		const frame_buffer& get_active_buffer() const
		{
			INT buffer_id;
			for (;;)
			{
				INT result = is_GetActSeqBuf(
					this->camera_handle,
					&buffer_id,
					nullptr,
					nullptr);
				if (result == IS_SUCCESS)
				{
					if (buffer_id > 0)
						break;

					log_warn(
						"is_GetActSeqBuf(%i, ...) in get_active_buffer() has buffer_id == %i",
						this->camera_handle,
						buffer_id);
				}
				else
				{
					detail::throw_on_error(camera_handle, result);
				}
			}

			return this->sequence[buffer_id - 1];
		}
	private:
		std::vector<frame_buffer> sequence;
		const HIDS camera_handle;
	};
}