#pragma once

// Forward declarations for all types
// 
// Use in your header files to gain access to all the declarations without worrying about the 
// details of declaring them. Forward declarations have a particularity, that they define the
// default arguments of a type. The reason for this is that template defaults can only be 
// defined once, and we want to be able to also benefit from these in the forward declarations
// as if we had included the file. The crstl files therefore have to include this file too and
// the details are hidden away here. It's a minor inconvenience for a good benefit

namespace crstl
{
	// allocator.h
	class allocator;

	// hash.h
	template<typename T> struct hash;

	// array.h
	template<typename T, size_t N> class array;

	// bitset.h
	template<size_t N, typename WordType> class bitset;

	// deque.h
	template<typename T, typename Allocator = crstl::allocator, size_t ChunkSize = 16> class deque;

	// filesystem.h
	class file;

	// fixed_deque.h
	template<typename T, size_t N> class fixed_deque;

	// fixed_function.h
	template<int SizeBytes, typename Return> class fixed_function;
	
	// fixed_open_hashmap.h
	template<typename Key, typename T, size_t NodeCount, typename Hasher = crstl::hash<Key>> class fixed_open_hashmap;
	template<typename Key, size_t NodeCount, typename Hasher = crstl::hash<Key>> class fixed_open_hashset;
	template<typename Key, typename T, size_t NodeCount, typename Hasher = crstl::hash<Key>> class fixed_open_multi_hashmap;
	template<typename Key, size_t NodeCount, typename Hasher = crstl::hash<Key>> class fixed_open_multi_hashset;

	// fixed_string.h
	template<typename T, int N> class basic_fixed_string;
	typedef basic_fixed_string<char, 8>   fixed_string8;
	typedef basic_fixed_string<char, 16>  fixed_string16;
	typedef basic_fixed_string<char, 32>  fixed_string32;
	typedef basic_fixed_string<char, 64>  fixed_string64;
	typedef basic_fixed_string<char, 128> fixed_string128;
	typedef basic_fixed_string<char, 256> fixed_string256;
	typedef basic_fixed_string<char, 512> fixed_string512;

	// fixed_vector.h
	template<typename T, size_t N> class fixed_vector;

	// intrusive_ptr.h
	template<typename T> class intrusive_ptr;

	// open_hashmap.h
	template<typename Key, typename T, typename Hasher = crstl::hash<Key>, typename Allocator = crstl::allocator> class open_hashmap;
	template<typename Key, typename Hasher = crstl::hash<Key>, typename Allocator = crstl::allocator> class open_hashset;
	template<typename Key, typename T, typename Hasher = crstl::hash<Key>, typename Allocator = crstl::allocator> class open_multi_hashmap;
	template<typename Key, typename Hasher = crstl::hash<Key>, typename Allocator = crstl::allocator> class open_multi_hashset;

	// pair.h
	template<typename T1, typename T2> class pair;

	// process.h
	class process;

	// span.h
	template<typename T> class span;

	// stack_vector.h
	template<typename T> class stack_vector;

	// string.h
	template <typename CharT, typename Allocator = crstl::allocator> class basic_string;
	typedef basic_string<char, allocator> string;
	typedef basic_string<wchar_t, allocator> wstring;

	// string_view.h
	template<typename CharT> class basic_string_view;
	typedef basic_string_view<char> string_view;
	typedef basic_string_view<wchar_t> wstring_view;

	// thread.h
	class thread;

	// timer.h
	class time;
	class timer;

	// unique_ptr.h
	template<typename T> class unique_ptr;

	// vector.h
	template<typename T, typename Allocator = crstl::allocator> class vector;

	// path.h
	template<typename StringInterface> class path_base;
	typedef path_base<string>          path;
	typedef path_base<fixed_string32>  fixed_path32;
	typedef path_base<fixed_string64>  fixed_path64;
	typedef path_base<fixed_string128> fixed_path128;
	typedef path_base<fixed_string256> fixed_path256;
	typedef path_base<fixed_string512> fixed_path512;
}