#pragma once

#include "crstl/config.h"

#include "crstl/config_fwd.h"

#include "crstl/type_builtins.h"

namespace crstl
{
	template<typename T, bool CanMemcpy = false>
	struct function_selector
	{
		static void copy_initialize_or_memcpy(T* destination, const T* source, size_t count)
		{
			for (size_t i = 0; i < count; ++i)
			{
				::new((void*)&destination[i]) T(source[i]);
			}
		}
	};

	template<typename T>
	struct function_selector<T, true>
	{
		static void copy_initialize_or_memcpy(T* destination, const T* source, size_t count)
		{
			memcpy(destination, source, sizeof(T) * count);
		}
	};

	template<typename T>
	void copy_initialize_or_memcpy(T* destination, const T* source, size_t count)
	{
		function_selector<T, crstl_is_trivially_copyable(T)>::copy_initialize_or_memcpy(destination, source, count);
	}
};