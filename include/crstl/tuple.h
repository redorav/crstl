#pragma once

#include "crstl/config.h"

#include "crstl/move_forward.h"

#include "crstl/type_builtins.h"

// tuple
//
// crstl::tuple is replacement for std::tuple
//

crstl_module_export namespace crstl
{
	// integer_sequence
	template <typename T, T... Integers>
	class integer_sequence
	{
	public:
		typedef T value_type;
		//static_assert(is_integral<T>::value, "crstl::integer_sequence can only be instantiated with an integral type");
		static crstl_constexpr size_t size() crstl_noexcept { return sizeof...(Integers); }
	};

	// An index sequence is integer_sequence with T as size_t

#if defined(CRSTL_COMPILER_GCC)
	template <typename T, T N>
	using make_integer_sequence = integer_sequence<T, __integer_pack(N)...>;
#elif defined(CRSTL_COMPILER_CLANG) || defined(CRSTL_COMPILER_MSVC)
	template <class T, T N>
	using make_integer_sequence = __make_integer_seq<integer_sequence, T, N>;
#else

	template <size_t N, typename IndexSequence>
	struct make_index_sequence_implementation;

	template <size_t N, size_t... Is>
	struct make_index_sequence_implementation<N, integer_sequence<size_t, Is...>>
	{
		typedef typename make_index_sequence_implementation<N - 1, integer_sequence<size_t, N - 1, Is...>>::type type;
	};

	template <size_t... Is>
	struct make_index_sequence_implementation<0, integer_sequence<size_t, Is...>>
	{
		typedef integer_sequence<size_t, Is...> type;
	};

	template <typename Target, typename Sequence>
	struct integer_sequence_convert_implementation;

	template <typename Target, size_t... Is>
	struct integer_sequence_convert_implementation<Target, integer_sequence<size_t, Is...>>
	{
		typedef integer_sequence<Target, Is...> type;
	};

	template <typename T, T N>
	struct make_integer_sequence_impl
	{
		typedef typename integer_sequence_convert_implementation<T, typename make_index_sequence_implementation<N, integer_sequence<size_t>>::type>::type type;
	};

	template <typename T, T N>
	using make_integer_sequence = typename make_integer_sequence_impl<T, N>::type;

#endif

	// Base declaration of tuple_leaf. Holds the value for each element in the tuple
	// A tuple is a collection of tuple_leafs
	template <size_t Index, typename ValueT, bool IsEmpty = crstl_is_empty(ValueT) && !crstl_is_final(ValueT)>
	class tuple_leaf;

	// Generic tuple_leaf implementation
	template<size_t Index, typename ValueT, bool IsEmpty>
	class tuple_leaf
	{
	public:

		tuple_leaf() : m_value() {}
		tuple_leaf(const ValueT& value) : m_value(value) {}
		tuple_leaf(ValueT&& value) : m_value(value) {}
		tuple_leaf(const tuple_leaf& other) : m_value(other.m_value) {}

		template<typename OtherValueT>
		explicit tuple_leaf(const tuple_leaf<Index, OtherValueT>& other) : m_value(other.value) {}

		template <typename OtherValueT>
		tuple_leaf& operator = (OtherValueT&& t)
		{
			m_value = crstl::forward<OtherValueT>(t);
			return *this;
		}

		ValueT& get() { return m_value; }
		const ValueT& get() const { return m_value; }

	private:

		tuple_leaf& operator = (const tuple_leaf& other) crstl_constructor_delete;

		ValueT m_value;
	};

	// Empty base class specialization
	template <size_t Index, typename ValueT>
	class tuple_leaf<Index, ValueT, true> : private ValueT
	{
	public:

		tuple_leaf() crstl_constructor_default;
		tuple_leaf(const ValueT& value) {}
		tuple_leaf(ValueT&& value) {}
		tuple_leaf(const tuple_leaf& other) : ValueT(other.get())
		{
			
		}

		template <typename OtherValueT>
		explicit tuple_leaf(OtherValueT&& t) : ValueT(forward<OtherValueT>(t)) {}

		template <typename OtherValueT>
		explicit tuple_leaf(const tuple_leaf<Index, OtherValueT>& t) : ValueT(t.get()) {}

		template <typename OtherValueT>
		tuple_leaf& operator = (OtherValueT&& t)
		{
			ValueT::operator = (forward<OtherValueT>(t));
			return *this;
		}

		ValueT& get() { return static_cast<ValueT&>(*this); }
		const ValueT& get() const { return static_cast<const ValueT&>(*this); }

	private:

		tuple_leaf& operator = (const tuple_leaf&) crstl_constructor_delete;
	};

	// Handles indices and types
	template <typename TupleIndices, typename... Ts>
	struct tuple_implementation;

	// e.g. tuple_implementation<0, 1, 2, int, float, char>
	// Multiple inheritance of tuple_leaf for every combination of Indices and Ts
	// Also guarantees that the order in which we add the elements is the order in
	// which they appear
	template<size_t... Indices, typename ... Ts>
	struct tuple_implementation<integer_sequence<size_t, Indices...>, Ts...> : public tuple_leaf<Indices, Ts>...
	{
		crstl_constexpr tuple_implementation() crstl_constructor_default;

		template <typename... ValueTs>
		explicit tuple_implementation(integer_sequence<size_t, Indices...>, ValueTs&&... values) : tuple_leaf<Indices, Ts>(crstl::forward<ValueTs>(values))... {}

		template <typename... ValueTs>
		explicit tuple_implementation(integer_sequence<size_t, Indices...>, const ValueTs&... values) : tuple_leaf<Indices, Ts>(values)... {}
	};

	template <typename... Ts> class tuple;

	// tuple owns a tuple_implementation and passes all parameters for it to handle it appropriately
	// This is because we convert the variadic tuple parameters into an integer sequence plus a parameter count,
	// then forward the parameters across
	template <typename T, typename... Ts>
	class tuple<T, Ts...>
	{
	public:
	
		crstl_constexpr tuple() crstl_constructor_default;
	
		tuple(T&& arg1, Ts&& ... args) : m_implementation(make_integer_sequence<size_t, sizeof...(Ts) + 1>{}, crstl::forward<T>(arg1), crstl::forward<Ts>(args)...) {}

		tuple(const T& arg1, const Ts& ... args) : m_implementation(make_integer_sequence<size_t, sizeof...(Ts) + 1>{}, arg1, args...) {}

		crstl_constexpr size_t size() const { return sizeof...(Ts) + 1; }
	
		tuple_implementation<make_integer_sequence<size_t, sizeof...(Ts) + 1>, T, Ts...> m_implementation;
	};
};