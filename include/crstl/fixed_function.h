#pragma once

#include "crstl/function_common.h"

// fixed_function
//
// fixed_function is a fixed-memory replacement for function
// Its main characteristics are:
//
// - The user supplies a size to the template object, which creates storage
// - The storage is used to locally copy or instantiate functors
// - fixed_function will static assert if storage is not enough to hold functor

crstl_module_export namespace crstl
{
	template<int SizeBytes, typename Signature>
	class fixed_function;

	template <typename> struct is_not_fixed_function { typedef const char* type; };

	template <int SizeBytes, typename Result, typename... Args>
	struct is_not_fixed_function<fixed_function<SizeBytes, Result(Args...)>> {};

	template<int SizeBytes, typename Result, typename... Args>
	class fixed_function<SizeBytes, Result(Args...)>
	{
	public:

		typedef Result result_type;

		fixed_function() crstl_noexcept : m_invoker(nullptr), m_manager(nullptr) {}

		template<typename FunctorT>
		fixed_function
		(
			FunctorT&& functor, 
			// Do not accept as Functor a fixed_function, we'll deal with it in a specialized copy/move constructor but due to overload
			// resolution rules, this one takes precedence. Remove reference to be able to compare the underlying type
			typename crstl::is_not_fixed_function<typename crstl::remove_reference<FunctorT>::type>::type = nullptr
		) crstl_noexcept
		{
			if (!handler<FunctorT>::empty_function(functor))
			{
				handler<FunctorT>::template create<FunctorT>(m_functor_storage, crstl::forward<FunctorT>(functor));
				m_invoker = &handler<FunctorT>::invoke;
				m_manager = &handler<FunctorT>::manage;
			}
			else
			{
				m_invoker = nullptr;
				m_manager = nullptr;
			}
		}

		fixed_function(fixed_function&& other) crstl_noexcept
		{
			m_functor_storage = other.m_functor_storage;
			m_invoker = other.m_invoker;
			m_manager = other.m_manager;
			other.m_invoker = nullptr;
			other.m_manager = nullptr;
		}

		fixed_function(const fixed_function& other) crstl_noexcept
		{
			copy(other);
		}

		template<int NewSizeBytes>
		fixed_function(const fixed_function<NewSizeBytes, Result(Args...)>& other) crstl_noexcept
		{
			static_assert(NewSizeBytes <= SizeBytes, "Not enough size to hold new function");
			(void)other;
			// TODO implement
		}

		~fixed_function() crstl_noexcept
		{
			if (m_manager)
			{
				m_manager(m_functor_storage, m_functor_storage, manager_operation::destroy);
			}
		}

		fixed_function& operator = (fixed_function&& other)
		{
			m_functor_storage = other.m_functor_storage;
			m_invoker = other.m_invoker;
			m_manager = other.m_manager;
			other.m_invoker = nullptr;
			other.m_manager = nullptr;
			return *this;
		}

		fixed_function& operator = (const fixed_function& other)
		{
			copy(other);
			return *this;
		}
		
		Result operator()(Args... args) const crstl_noexcept
		{
			crstl_assert(m_invoker != nullptr);
			return m_invoker(m_functor_storage, crstl::forward<Args>(args)...);
		}

		explicit operator bool() const crstl_noexcept
		{
			return m_invoker != nullptr;
		}

	private:

		template<typename FunctorT>
		using handler = functor_handler<Result(Args...), FunctorT, SizeBytes>;

		// The invoker_type is a function pointer that returns Result and takes functor_storage plus a variable number of arguments
		using invoker_type = Result(*)(const functor_storage<SizeBytes>&, Args&&...);

		// The manager_type is a function pointer that returns void and takes a destination (non-const) and source (const) functor_storage
		using manager_type = void(*)(functor_storage<SizeBytes>&, const functor_storage<SizeBytes>&, manager_operation::t);

		void copy(const fixed_function& other) crstl_noexcept
		{
			if (other.m_manager)
			{
				other.m_manager(m_functor_storage, other.m_functor_storage, manager_operation::copy);
			}

			m_manager = other.m_manager;
			m_invoker = other.m_invoker;
		}

		// The invoker is a pointer to a function that reinterprets the data we pass in and calls the function
		// It returns the same value as the fixed_function
		invoker_type m_invoker;

		// The manager can perform operations such as copy, move, create and destroy by reinterpreting the data
		manager_type m_manager;

		// Storage for the function object
		functor_storage<SizeBytes> m_functor_storage;
	};
};