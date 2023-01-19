#pragma once

#include "config.h"

#include "type_utils.h"

namespace crstl
{
	namespace manager_operation
	{
		enum t
		{
			destroy,
			copy
		};
	}

	class undefined_class;

	union callable_types
	{
		void* object;
		void(*function)(void);
		void(undefined_class::*member)(void);
	};

	template<int Size>
	union functor_storage
	{
		static_assert(Size >= 0, "Cannot have a negative size");

		void* data() crstl_noexcept { return &m_data[0]; }
		const void* data() const crstl_noexcept { return &m_data[0]; }

		template<typename T>
		T& data() crstl_noexcept { return *static_cast<T*>(data()); }

		template<typename T>
		const T& data() const crstl_noexcept { return *static_cast<const T*>(data()); }

		char m_data[Size == 0 ? sizeof(callable_types) : Size];
	};

	template<typename Signature, typename Functor, int Size>
	class functor_handler;

	template<typename Result, typename Functor, int Size, typename... Args>
	class functor_handler<Result(Args...), Functor, Size>
	{
	public:

		// If we request a specific size, then we'll consider it local, without the option to make a heap allocation
		// If we pass in 0, then we have some stack space to at least store function pointers on the stack
		static const bool local = Size > 0 || sizeof(Functor) <= sizeof(const functor_storage<0>);

		static Functor* get_pointer(const functor_storage<Size>& source)
		{
			crstl_constexpr_if(local)
			{
				const Functor& functor = source.template data<Functor>();
				return const_cast<Functor*>(&(functor));
			}
			else
			{
				return source.template data<Functor*>();
			}
		}

		template<typename Fn>
		static void create(functor_storage<Size>& destination, Fn&& fn)
		{
			static_assert(local ? sizeof(Functor) <= sizeof(functor_storage<Size>) : true, "Not enough space to store functor");

			crstl_constexpr_if(local)
			{
				::new (destination.data()) Functor(crstl::forward<Fn>(fn));
			}
			else
			{
				destination.template data<Functor*>() = new Functor(crstl::forward<Fn>(fn));
			}
		}

		static void destroy(functor_storage<Size>& destination)
		{
			crstl_constexpr_if(local) // If local, just call destructor 
			{
				destination.template data<Functor>().~Functor();
			}
			else
			{
				delete destination.template data<Functor*>();
			}
		}

		template<typename Fn>
		static void init_functor(functor_storage<Size>& functor, Fn&& f) crstl_noexcept
		{
			create(functor, crstl::forward<Fn>(f));
		}

		static Result invoke(const functor_storage<Size>& functor, Args&&... args)
		{
			return (*get_pointer(functor))(crstl::forward<Args>(args)...);
		}

		static void manage(functor_storage<Size>& destination, const functor_storage<Size>& source, manager_operation::t operation)
		{
			switch (operation)
			{
				case manager_operation::destroy:
				{
					destroy(destination);
					break;
				}
				case manager_operation::copy:
				{
					init_functor(destination, *static_cast<const Functor*>(get_pointer(source)));
					break;
				}
			}
		}

		template<typename T>
		static bool empty_function(T* fp) noexcept
		{
			return fp == nullptr;
		}

		template<typename Class, typename T>
		static bool empty_function(T Class::* mp) noexcept
		{
			return mp == nullptr;
		}

		template<typename T>
		static bool empty_function(const T&) noexcept
		{
			return false;
		}
	};
};