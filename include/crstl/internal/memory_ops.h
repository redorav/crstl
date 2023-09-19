#pragma once

#include "crstl/config.h"

#include "crstl/config_fwd.h"

#include "crstl/type_builtins.h"

#include "crstl/crstldef.h"

extern "C"
{
#if defined(CRSTL_COMPILER_MSVC)
	void* memcpy(void* destination, void const* source, size_t size);
	crstl_2015_dllimport void* memmove(void* destination, const void* source, size_t size);

	void* memset(void* dst, int val, size_t size);
#endif

	wchar_t* wmemset(wchar_t* ptr, wchar_t wc, size_t num) crstl_linux_wthrow;
};

namespace crstl
{
	//------------------
	// Memory Primitives
	//------------------

	inline void memory_copy(void* crstl_restrict destination, const void* crstl_restrict source, size_t size)
	{
	#if defined(CRSTL_COMPILER_MSVC)
		memcpy(destination, source, size);
	#else
		__builtin_memcpy(destination, source, size);
	#endif
	}

	inline void memory_move(void* destination, const void* source, size_t size)
	{
	#if defined(CRSTL_COMPILER_MSVC)
		memmove(destination, source, size);
	#else
		__builtin_memmove(destination, source, size);
	#endif
	}

	inline void memory_set(void* destination, int value, size_t count)
	{
	#if defined(CRSTL_COMPILER_MSVC)
		memset(destination, value, count);
	#else
		__builtin_memset(destination, value, count);
	#endif
	}

	inline void wmemory_set(wchar_t* destination, wchar_t value, size_t count)
	{
		wmemset(destination, value, count);
	}

	//---------------------------------------------------
	// Default Initialization: default initialize a range
	//---------------------------------------------------

	template<typename T, bool CanMemset = false>
	struct default_initialize_or_memset_zero_select
	{
		static void default_initialize_or_memset_zero(T* crstl_restrict destination, size_t count)
		{
			for (size_t i = 0; i < count; ++i)
			{
				crstl_placement_new((void*)&destination[i]) T();
			}
		}
	};

	template<typename T>
	struct default_initialize_or_memset_zero_select<T, true>
	{
		static void default_initialize_or_memset_zero(T* crstl_restrict destination, size_t count)
		{
			memory_set(destination, 0, sizeof(T) * count);
		}
	};

	template<typename T>
	void default_initialize_or_memset_zero(T* crstl_restrict destination, size_t count)
	{
		default_initialize_or_memset_zero_select<T, crstl_is_trivially_constructible(T)>::default_initialize_or_memset_zero(destination, count);
	}

	//--------------------------------------------------
	// Set Initialization: Set a range to the same value
	//--------------------------------------------------

	namespace memset_operation
	{
		enum t
		{
			placement_copy = 0,
			memory_copy = 1,
			memory_set = 2
		};
	};

	template<typename T, memset_operation::t op = memset_operation::placement_copy>
	struct set_initialize_or_memset_select
	{
		static void set_initialize_or_memset(T* crstl_restrict destination, const T& value, size_t count)
		{
			for (size_t i = 0; i < count; ++i)
			{
				crstl_placement_new(&destination[i]) T(value);
			}
		}
	};

	template<typename T>
	struct set_initialize_or_memset_select<T, memset_operation::memory_copy>
	{
		static void set_initialize_or_memset(T* crstl_restrict destination, const T& value, size_t count)
		{
			for (size_t i = 0; i < count; ++i)
			{
				destination[i] = value;
			}
		}
	};
	
	template<typename T>
	struct set_initialize_or_memset_select<T, memset_operation::memory_set>
	{
		static void set_initialize_or_memset(T* crstl_restrict destination, T value, size_t count)
		{
			static_assert(sizeof(value) == 1, "memset copies bytes");
			memory_set(destination, value, count);
		}
	};

	template<typename T>
	void set_initialize_or_memset(T* crstl_restrict destination, const T& value, size_t count)
	{
		set_initialize_or_memset_select<T,
			crstl_is_trivially_constructible(T) && (sizeof(T) == 1) ? memset_operation::memory_set :
			crstl_is_trivially_copyable(T) ? memset_operation::memory_copy :
			memset_operation::placement_copy
		>::set_initialize_or_memset(destination, value, count);
	}

	//-------------------------------------------------
	// Copy Initialization: Copy entire range of memory
	//-------------------------------------------------

	template<typename T, bool CanMemcpy = false>
	struct copy_initialize_or_memcpy_select
	{
		static void copy_initialize_or_memcpy(T* crstl_restrict destination, const T* crstl_restrict source, size_t count)
		{
			for (size_t i = 0; i < count; ++i)
			{
				crstl_placement_new((void*)&destination[i]) T(source[i]);
			}
		}
	};

	template<typename T>
	struct copy_initialize_or_memcpy_select<T, true>
	{
		static void copy_initialize_or_memcpy(T* crstl_restrict destination, const T* crstl_restrict source, size_t count)
		{
			memory_copy(destination, source, sizeof(T) * count);
		}
	};

	template<typename T>
	void copy_initialize_or_memcpy(T* crstl_restrict destination, const T* crstl_restrict source, size_t count)
	{
		copy_initialize_or_memcpy_select<T, crstl_is_trivially_copyable(T)>::copy_initialize_or_memcpy(destination, source, count);
	}
};