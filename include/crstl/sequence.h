#pragma once

#include "crstl/config.h"

#include "crstl/type_builtins.h"

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

	// An index sequence is an integer sequence with size_t

	template <size_t... Ts>
	using index_sequence = integer_sequence<size_t, Ts...>;

	template <size_t Size>
	using make_index_sequence = make_integer_sequence<size_t, Size>;

	template <class... Ts>
	using index_sequence_for = make_index_sequence<sizeof...(Ts)>;
};