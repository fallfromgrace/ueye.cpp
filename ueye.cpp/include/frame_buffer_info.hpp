#pragma once

#include "uEye.h"

#include "includes.hpp"

namespace ids
{
	// 
	class frame_buffer_info
	{
	public:
		// 
		frame_buffer_info(
			INT buffer_id,
			char* buffer_data) :
			buffer_id(buffer_id),
			buffer_data(buffer_data)
		{

		}

		// 
		INT id() const
		{
			return this->buffer_id;
		}

		// 
		char* data() const
		{
			return this->buffer_data;
		}

	private:
		INT buffer_id;
		char* buffer_data;
	};
}