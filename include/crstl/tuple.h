#pragma once

#include "crstl/config.h"

#include "crstl/move_forward.h"

#include "crstl/type_builtins.h"

#include "crstl/sequence.h"

// tuple
//
// crstl::tuple is replacement for std::tuple
//

crstl_module_export namespace crstl
{
#if defined(CRSTL_COMPILER_CLANG)

	template<int Index, typename ... Args>
	struct type_pack_element
	{
		typedef __type_pack_element<Index, Args...> type;
	};

#else

	namespace internal
	{
		// https://ldionne.com/2015/11/29/efficient-parameter-pack-indexing/
		template <int Index, typename T> struct parameter_pack_indexed { using type = T; };

		template <int Index, typename T>
		static parameter_pack_indexed<Index, T> parameter_pack_select(parameter_pack_indexed<Index, T>);

		template <typename Indices, typename ... Ts> struct parameter_pack_indexer;

		template <int ... Indices, typename ... Ts>
		struct parameter_pack_indexer<crstl::integer_sequence<size_t, Indices...>, Ts...> : parameter_pack_indexed<Indices, Ts>... {};
	};

	template<int Index, typename ... Ts>
	struct type_pack_element
	{
		typedef typename decltype(internal::parameter_pack_select<Index>(internal::parameter_pack_indexer<crstl::index_sequence_for<Ts...>, Ts...>{}))::type type;
	};

#endif

	// Base declaration of tuple_leaf. Holds the value for each element in the tuple
	// A tuple is a collection of tuple_leafs, with an associated index for each leaf
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

	// Empty base class specialization for tuple_leaf
	template <size_t Index, typename ValueT>
	class tuple_leaf<Index, ValueT, true> : private ValueT
	{
	public:

		tuple_leaf() crstl_constructor_default;
		tuple_leaf(const ValueT& value) {}
		tuple_leaf(ValueT&& value) {}
		tuple_leaf(const tuple_leaf& other) : ValueT(other.get()) {}

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
	// Multiple inheritance of tuple_leaf for every combination of Indices and Ts, so every member is laid out consecutively
	// This guarantees that the order in which we add the elements is the order in which they appear. Each constructor then
	// calls the parent constructors in sequence as well
	template<size_t ... Indices, typename ... Ts>
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

		// This does not conform to the standard but is a lot more convenient than the free get<>() function
		template<int Index>
		const typename type_pack_element<Index, T, Ts...>::type& get()
		{
			return static_cast<tuple_leaf<Index, typename type_pack_element<Index, T, Ts...>::type>&>(m_implementation).get();
		}

		crstl_constexpr size_t size() const { return sizeof...(Ts) + 1; }
	
		tuple_implementation<make_integer_sequence<size_t, sizeof...(Ts) + 1>, T, Ts...> m_implementation;
	};

	// We get the type from the index using the type_pack_element helper, and using that type we cast
	// m_implementation to the tuple_leaf with the corresponding index and type. Since m_implementation
	// derives from all the possible tuple_leafs, we get a valid object and just call get() on it
	template<int Index, typename... Ts>
	typename type_pack_element<Index, Ts...>::type& get(tuple<Ts ...>& t)
	{
		return static_cast<tuple_leaf<Index, typename type_pack_element<Index, Ts...>::type>&>(t.m_implementation).get();
	}

	template<int Index, typename... Ts>
	const typename type_pack_element<Index, Ts...>::type& get(const tuple<Ts ...>& t)
	{
		return static_cast<tuple_leaf<Index, typename type_pack_element<Index, Ts...>::type>&>(t.m_implementation).get();
	}

	template<int Index, typename... Ts>
	typename type_pack_element<Index, Ts...>::type&& get(tuple<Ts ...>&& t)
	{
		return static_cast<tuple_leaf<Index, typename type_pack_element<Index, Ts...>::type>&>(crstl::move(t.m_implementation)).get();
	}
};