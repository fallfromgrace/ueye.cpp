#ifndef LIBIDS_EZ_IDS_CAMERA_FACTORY_HPP
#define LIBIDS_EZ_IDS_CAMERA_FACTORY_HPP

#include <algorithm>
#include <vector>

#include "uEye.h"

#include "includes.hpp"
#include "error.hpp"

namespace ids
{
	namespace detail
	{
		// Encapsulates the allocation of the UEYE_CAMERA_LIST structure.
		class pueye_camera_list_guard
		{
		public:
			// 
			pueye_camera_list_guard(size_t camera_count)
			{
				size_t camera_list_size = sizeof(UEYE_CAMERA_LIST) +
					std::max(camera_count - 1, static_cast<size_t>(0)) *
					sizeof(UEYE_CAMERA_INFO);
				camera_list_ptr = malloc(camera_list_size);
				if (camera_list_ptr == nullptr)
					throw std::runtime_error("");
				static_cast<PUEYE_CAMERA_LIST>(this->camera_list_ptr)->dwCount = camera_count;
			}

			// 
			pueye_camera_list_guard(const pueye_camera_list_guard& other) = delete;

			// 
			pueye_camera_list_guard& operator=(const pueye_camera_list_guard&& other) = delete;

			// 
			~pueye_camera_list_guard()
			{
				free(camera_list_ptr);
				camera_list_ptr = nullptr;
			}

			// 
			const PUEYE_CAMERA_LIST camera_list() const
			{
				return static_cast<PUEYE_CAMERA_LIST>(this->camera_list_ptr);
			}

		private:
			void* camera_list_ptr;
		};
	}

	// 
	std::vector<UEYE_CAMERA_INFO> get_cameras()
	{
		std::vector<UEYE_CAMERA_INFO> cameras;

		INT result;
		INT camera_count;
		result = is_GetNumberOfCameras(&camera_count);
		if (result != 0)
			throw ids_exception(0, result);

		if (camera_count > 0)
		{
			detail::pueye_camera_list_guard camera_list_guard(static_cast<size_t>(camera_count));
			result = is_GetCameraList(camera_list_guard.camera_list());
			if (result != 0)
				throw ids_exception(0, result);

			for (int i = 0; i < camera_count; i++)
				cameras.push_back(camera_list_guard.camera_list()->uci[i]);
		}

		return cameras;
	}
}

#endif//LIBIDS_EZ_IDS_CAMERA_FACTORY_HPP