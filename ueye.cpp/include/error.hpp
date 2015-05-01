#ifndef LIBIDS_EZ_IDS_HELPERS_HPP
#define LIBIDS_EZ_IDS_HELPERS_HPP

#include <stdexcept>
#include <string>

#include "uEye.h"

#include "includes.hpp"
#include "logging\logging.hpp"

namespace ids
{
	namespace detail
	{
		std::string get_error_message(HIDS camera_handle);
	}

	class ids_exception : public std::runtime_error
	{
	public:
		ids_exception(HIDS camera_handle, INT result) :
			std::runtime_error(detail::get_error_message(camera_handle)),
			result(result)
		{

		}

		INT code() const
		{
			return result;
		}
	private:
		INT result;
	};

	namespace detail
	{
		// 
		void throw_on_error(HIDS camera_handle, INT result)
		{
			if (result != IS_SUCCESS)
				throw ids_exception(camera_handle, result);
		}

		// 
		std::string get_error_message(HIDS camera_handle)
		{
			INT err;
			IS_CHAR* pcerr;
			INT result = is_GetError(camera_handle, &err, &pcerr);
			if (result != IS_SUCCESS)
				log_error(
					"is_GetError(%i,%i,%i) failed in get_error_message(%i) with code %i\n",
					camera_handle,
					err,
					pcerr,
					camera_handle,
					result);
			return std::string(pcerr);
		}
	}
}

#endif//LIBIDS_EZ_IDS_HELPERS_HPP