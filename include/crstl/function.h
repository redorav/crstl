#pragma once

#include "crstl/function_common.h"

// function
//
// function is a fixed-memory replacement for std::function
//
// - function has a small configurable buffer

// 0 has a special value in functor_storage that means the minimum to store a pointer, but it is customizable
#if !defined(CRSTL_FUNCTION_INTERNAL_BUFFER_BYTES)
#define CRSTL_FUNCTION_INTERNAL_BUFFER_BYTES 0
#endif

crstl_module_export namespace crstl
{
	template<typename Signature>
	class function;

	template <typename> struct is_not_function { typedef const char* type; };

	template <typename Result, typename... Args>
	struct is_not_function<function<Result(Args...)>> {};

	template<typename Result, typename... Args>
	class function<Result(Args...)>
	{
		typedef Result result_type;

	public:

		function() crstl_noexcept : m_invoker(nullptr), m_manager(nullptr) {}

		template<typename FunctorT>
		function
		(
			FunctorT&& functor,
			// Do not accept as Functor a function, we'll deal with it in a specialized copy/move constructor but due to overload
			// resolution rules, this one takes precedence. Remove reference to be able to compare the underlying type
			typename crstl::is_not_function<typename crstl::remove_reference<FunctorT>::type>::type = nullptr
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

		function(function&& other) crstl_noexcept
		{
			if (this != &other)
			{
				m_functor_storage = other.m_functor_storage;
				m_invoker = other.m_invoker;
				m_manager = other.m_manager;
				other.m_invoker = nullptr;
				other.m_manager = nullptr;
			}
		}

		function(const function& other) crstl_noexcept
		{
			if (this != &other)
			{
				copy(other);
			}
		}

		~function() crstl_noexcept
		{
			if (m_manager)
			{
				m_manager(&m_functor_storage, &m_functor_storage, manager_operation::destroy);
			}
		}

		function& operator = (function&& other)
		{
			m_functor_storage = other.m_functor_storage;
			m_invoker = other.m_invoker;
			m_manager = other.m_manager;
			other.m_invoker = nullptr;
			other.m_manager = nullptr;
			return *this;
		}

		function& operator = (const function& other)
		{
			copy(other);
			return *this;
		}

		Result operator()(Args... args) const crstl_noexcept
		{
			crstl_assert(m_invoker != nullptr);
			return m_invoker(&m_functor_storage, crstl::forward<Args>(args)...);
		}

		explicit operator bool() const crstl_noexcept
		{
			return m_invoker != nullptr;
		}

	private:

		template<typename FunctorT>
		using handler = functor_handler<Result(Args...), FunctorT, CRSTL_FUNCTION_INTERNAL_BUFFER_BYTES, true>;

		// The invoker_type is a function pointer that returns Result and takes functor_storage plus a variable number of arguments
		using invoker_type = Result(*)(const void*, Args&&...);

		// The manager_type is a function pointer that returns void and takes a destination (non-const) and source (const) functor_storage
		using manager_type = void(*)(void*, const void*, manager_operation::t);

		void copy(const function& other) crstl_noexcept
		{
			if (other.m_manager)
			{
				other.m_manager(&m_functor_storage, &other.m_functor_storage, manager_operation::copy);
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
		functor_storage<CRSTL_FUNCTION_INTERNAL_BUFFER_BYTES> m_functor_storage;
	};
};