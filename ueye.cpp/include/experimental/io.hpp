#pragma once

#include "uEye.h"

#include "includes.hpp"
#include "error.hpp"

namespace ids
{
	//// 
	//class io_property
	//{
	//public:
	//	//
	//	io_property(HIDS camera_handle) :
	//		camera_handle(camera_handle)
	//	{

	//	}
	//protected:
	//	INT get(UINT command, INT param)
	//	{
	//		INT result = is_IO(
	//			this->camera_handle,
	//			command,
	//			&param,
	//			sizeof(param));
	//		throw_on_error(this->camera_handle, result);
	//		return param;
	//	}
	//private:
	//	HIDS camera_handle;
	//};
}