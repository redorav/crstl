#pragma once

#include "function_common.h"

// fixed_function
//
// fixed_function is a fixed-memory replacement for function
// Its main characteristics are:
//
// - The user supplies a size to the template object, which creates storage
// - The storage is used to locally copy or instantiate functors
// - fixed_function will static assert if storage is not enough to hold functor

namespace crstl
{
	template<typename Signature, int SizeBytes>
	class fixed_function;

	template<typename Result, typename... Args, int SizeBytes>
	class fixed_function<Result(Args...), SizeBytes>
	{
	public:

		typedef Result result_type;

		fixed_function() crstl_noexcept : m_invoker(nullptr), m_manager(nullptr) {}

		template<typename Functor>
		fixed_function(Functor&& functor) crstl_noexcept
		{
			if (!handler<Functor>::empty_function(functor))
			{
				handler<Functor>::template create<Functor>(m_functor_storage, crstl::forward<Functor>(functor));
				m_invoker = &handler<Functor>::invoke;
				m_manager = &handler<Functor>::manage;
			}
			else
			{
				m_invoker = nullptr;
				m_manager = nullptr;
			}
		}

		fixed_function(fixed_function&& function) crstl_noexcept
		{
			m_functor_storage = function.m_functor_storage;
			m_invoker = function.m_invoker;
			m_manager = function.m_manager;
			function.m_invoker = nullptr;
			function.m_manager = nullptr;
		}

		fixed_function(fixed_function& function) crstl_noexcept
		{
			copy(function);
		}

		fixed_function(const fixed_function& function) crstl_noexcept
		{
			copy(function);
		}

		~fixed_function() crstl_noexcept
		{
			if (m_manager)
			{
				m_manager(m_functor_storage, m_functor_storage, manager_operation::destroy);
			}
		}

		fixed_function& operator = (fixed_function&& function)
		{
			m_functor_storage = function.m_functor_storage;
			m_invoker = function.m_invoker;
			m_manager = function.m_manager;
			function.m_invoker = nullptr;
			function.m_manager = nullptr;
			return *this;
		}

		fixed_function& operator = (const fixed_function& function)
		{
			copy(function);
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

		template<typename Functor>
		using handler = functor_handler<Result(Args...), Functor, SizeBytes>;

		using invoker_type = Result(*)(const functor_storage<SizeBytes>&, Args&&...);
		using manager_type = void(*)(functor_storage<SizeBytes>&, const functor_storage<SizeBytes>&, manager_operation::t);

		void copy(const fixed_function& f) crstl_noexcept
		{
			f.m_manager(m_functor_storage, f.m_functor_storage, manager_operation::copy);
			m_manager = f.m_manager;
			m_invoker = f.m_invoker;
		}

		// The invoker is a pointer to a function that reinterprets the data we pass in and calls the function
		// It return the same value as the fixed_function
		invoker_type m_invoker;

		// The manager can perform operations such as copy, move, create and destroy by reinterpreting the data
		manager_type m_manager;

		// Storage for the function object
		functor_storage<SizeBytes> m_functor_storage;
	};
};