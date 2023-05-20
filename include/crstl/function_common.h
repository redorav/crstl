#pragma once

#include "crstl/config.h"

#include "crstl/move_forward.h"

#include "crstl/type_utils.h"

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

	// functor_storage works as either a pointer to a memory block in the heap or a memory
	// block itself, depending on whether the captures of the lambda fit in m_data
	template<int Size>
	union functor_storage
	{
		static_assert(Size >= 0, "Cannot have a negative size");

		// Effectively returns this
		void* data() crstl_noexcept { return &m_data[0]; }
		const void* data() const crstl_noexcept { return &m_data[0]; }

		template<typename T>
		T& data() crstl_noexcept { return *static_cast<T*>(data()); }

		template<typename T>
		const T& data() const crstl_noexcept { return *static_cast<const T*>(data()); }

		char m_data[Size == 0 ? sizeof(callable_types) : Size];
	};

	template<typename Signature, typename FunctorT, int Size>
	class functor_handler;

	template<typename Result, typename FunctorT, int Size, typename... Args>
	class functor_handler<Result(Args...), FunctorT, Size>
	{
	public:

		// If we request a specific size, then we'll consider it local, without the option to make a heap allocation
		// If we pass in 0, then we have some stack space to at least store function pointers on the stack
		static const bool local = Size > 0 || sizeof(FunctorT) <= sizeof(const functor_storage<0>);

		static FunctorT* get_pointer(const functor_storage<Size>& source)
		{
			crstl_constexpr_if(local)
			{
				const FunctorT& functor = source.template data<FunctorT>();
				return const_cast<FunctorT*>(&(functor));
			}
			else
			{
				return source.template data<FunctorT*>();
			}
		}

		template<typename Fn>
		static void create(functor_storage<Size>& destination, Fn&& fn)
		{
			static_assert(local ? sizeof(FunctorT) <= sizeof(destination) : true, "Not enough space to store functor");

			crstl_constexpr_if(local)
			{
				::new (destination.data()) FunctorT(crstl::forward<Fn>(fn));
			}
			else
			{
				destination.template data<FunctorT*>() = new FunctorT(crstl::forward<Fn>(fn));
			}
		}

		static void destroy(functor_storage<Size>& destination)
		{
			crstl_constexpr_if(local) // If local, just call destructor 
			{
				destination.template data<FunctorT>().~FunctorT();
			}
			else
			{
				delete destination.template data<FunctorT*>();
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
					init_functor(destination, *static_cast<const FunctorT*>(get_pointer(source)));
					break;
				}
			}
		}

		// A null function pointer is considered empty
		template<typename T>
		static bool empty_function(T* function_pointer) noexcept
		{
			return function_pointer == nullptr;
		}

		// A null member function is empty
		template<typename Class, typename T>
		static bool empty_function(T Class::* member_pointer) noexcept
		{
			return member_pointer == nullptr;
		}

		// A reference is never empty
		template<typename T>
		static bool empty_function(const T&) noexcept
		{
			return false;
		}
	};
};