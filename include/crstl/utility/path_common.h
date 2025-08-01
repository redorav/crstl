#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

#include "crstl/utility/string_common.h"

crstl_module_export namespace crstl
{
	// Forward declarations to be able to convert between these
	template<typename CharT, typename Allocator> class basic_string;
	template<typename CharT, int NumElements> class basic_fixed_string;

	template<typename StringInterface>
	class path_base
	{
	public:

		typedef typename StringInterface::const_pointer const_pointer;
		typedef typename StringInterface::value_type CharT;

		static const size_t npos = (size_t)-1;

		path_base() {}

		path_base(const char* path) : m_path_string(path) { normalize(); }

		path_base(const char* path, size_t count) : m_path_string(path, count) { normalize(); }

		path_base(const char* path, size_t offset, size_t count) : m_path_string(path + offset, path + offset + count) { normalize(); }

		template<typename Allocator>
		path_base(const basic_string<char, Allocator>& s) : path_base(s.c_str(), s.length()) {}

		template<int NumElements>
		path_base(const basic_fixed_string<char, NumElements>& s) : path_base(s.c_str(), s.length()) {}

		path_base(const path_base& path, size_t count) : path_base(path.c_str(), count) {}

		path_base(const path_base& path, size_t offset, size_t count) : path_base(path.c_str(), offset, count) {}

		template <typename CharType>
		path_base& append_convert(const CharType* other)
		{
			m_path_string.append_convert(other); return *this;
		}

		crstl_constexpr size_t capacity() const crstl_noexcept
		{
			return m_path_string.capacity();
		}

		const char* c_str() const { return m_path_string.c_str(); }

		int compare(const char* s) const crstl_noexcept
		{
			return m_path_string.compare(s);
		}

		int comparei(const char* s) const crstl_noexcept
		{
			return m_path_string.comparei(s);
		}

		bool empty() const crstl_noexcept
		{
			return m_path_string.empty();
		}

		path_base extension() const
		{
			size_t lastDot;
			if (has_extension_internal(lastDot))
			{
				return path_base(*this, lastDot, m_path_string.length() - lastDot);
			}
			else
			{
				return path_base();
			}
		}

		path_base filename() const
		{
			size_t lastSeparator = m_path_string.find_last_of("/");
			return path_base(*this, lastSeparator + 1, m_path_string.length() - (lastSeparator + 1));
		}

		//--------------
		// find_first_of
		//--------------

		crstl_constexpr14 size_t find_first_of(const_pointer needle_string, size_t pos, size_t needle_length) const crstl_noexcept
		{
			return m_path_string.find_first_of(needle_string, pos, needle_length);
		}

		crstl_constexpr14 size_t find_first_of(const_pointer needle_string, size_t pos = 0) const crstl_noexcept
		{
			return m_path_string.find_first_of(needle_string, pos);
		}

		crstl_constexpr14 size_t find_first_of(const StringInterface& needle_string, size_t pos = 0) const crstl_noexcept
		{
			return m_path_string.find_first_of(needle_string, pos);
		}

		crstl_constexpr14 size_t find_first_of(CharT c, size_t pos = 0) const crstl_noexcept
		{
			return m_path_string.find(c, pos);
		}

		//-------------
		// find_last_of
		//-------------

		crstl_constexpr14 size_t find_last_of(const_pointer needle_string, size_t pos, size_t needle_length) const crstl_noexcept
		{
			return m_path_string.find_last_of(needle_string, pos, needle_length);
		}

		crstl_constexpr14 size_t find_last_of(const_pointer needle_string, size_t pos = npos) const crstl_noexcept
		{
			return m_path_string.find_last_of(needle_string, pos);
		}

		crstl_constexpr14 size_t find_last_of(const StringInterface& needle_string, size_t pos = npos) const crstl_noexcept
		{
			return m_path_string.find_last_of(needle_string, pos);
		}

		crstl_constexpr14 size_t find_last_of(CharT c, size_t pos = npos) const crstl_noexcept
		{
			return m_path_string.rfind(c, pos);
		}

		bool has_extension() const
		{
			size_t lastDot;
			return has_extension_internal(lastDot);
		}

		size_t length() const
		{
			return m_path_string.length();
		}

		path_base parent_path() const
		{
			size_t lastSeparator = m_path_string.find_last_of("/");

			if (lastSeparator != m_path_string.npos)
			{
				return path_base(*this, lastSeparator);
			}
			else
			{
				return path_base();
			}
		}

		path_base& remove_filename()
		{
			size_t lastSeparator = m_path_string.find_last_of("/");
			if (lastSeparator != m_path_string.length() - 1)
			{
				m_path_string.resize(lastSeparator + 1); // Include the separator
			}

			return *this;
		}

		// https://en.cppreference.com/w/cpp/filesystem/path/replace_extension
		// Replaces the extension with replacement or removes it when the default value of replacement is used.
		// Firstly, if this path has an extension(), it is removed from the generic-format view of the pathname.
		// Then, a dot character is appended to the generic-format view of the pathname, if replacement is not empty and does not begin with a dot character.
		// Then replacement is appended as if by operator += (replacement).
		path_base& replace_extension(const char* extension)
		{
			size_t lastDot;
			if (has_extension_internal(lastDot))
			{
				m_path_string.resize(lastDot);
			}

			if (extension && extension[0] != '\0')
			{
				if (extension[0] != '.')
				{
					m_path_string += ".";
				}

				m_path_string += extension;
			}

			return *this;
		}

		void resize(size_t n)
		{
			m_path_string.resize(n);
		}

		void resize(size_t n, char c)
		{
			m_path_string.resize(n, c);
		}

		crstl_constexpr14 bool operator == (const char* path) const
		{
			return m_path_string == path;
		}

		crstl_constexpr14 bool operator == (const path_base& path) const
		{
			return m_path_string == path.m_path_string;
		}

		crstl_constexpr14 bool operator != (const char* path) const
		{
			return m_path_string != path;
		}

		crstl_constexpr14 bool operator != (const path_base& path) const
		{
			return m_path_string != path.m_path_string;
		}

		crstl_constexpr14 CharT& operator [](size_t i) crstl_noexcept
		{
			return crstl_assert(i < length()), m_path_string[i];
		}

		crstl_constexpr const CharT& operator [](size_t i) const crstl_noexcept
		{
			return crstl_assert(i < length()), m_path_string[i];
		}

		crstl_constexpr14 path_base operator + (const char* str) const
		{
			path_base new_path;
			new_path.m_path_string.reserve(m_path_string.length() + string_length(str));
			new_path.m_path_string = this->m_path_string;
			new_path.m_path_string += str;
			return new_path;
		}

		crstl_constexpr14 path_base operator + (const path_base& path) const
		{
			path_base new_path;
			new_path.m_path_string.reserve(m_path_string.length() + path.m_path_string.length());
			new_path.m_path_string = this->m_path_string;
			new_path += path.m_path_string;
			return *this + path.m_path_string;
		}

		crstl_constexpr14 path_base& operator += (const char* str)
		{
			m_path_string += str;
			return *this;
		}

		crstl_constexpr14 path_base& operator += (const path_base& path)
		{
			m_path_string += path.m_path_string;
			return *this;
		}

		crstl_constexpr14 path_base operator / (const char* str) const
		{
			path_base new_path = *this;
			new_path.add_trailing_separator();
			new_path.m_path_string += str;
			return new_path;
		}

		crstl_constexpr14 path_base operator / (const path_base& path) const
		{
			path_base newPath = *this;
			newPath.add_trailing_separator();
			newPath.m_path_string += path.m_path_string;
			return newPath;
		}

		crstl_constexpr14 path_base& operator /= (const char* str)
		{
			add_trailing_separator();
			m_path_string += str;
			return *this;
		}

		crstl_constexpr14 path_base& operator /= (const path_base& path)
		{
			add_trailing_separator();
			m_path_string += path.m_path_string;
			return *this;
		}

	private:

		// https://en.cppreference.com/w/cpp/filesystem/path/extension
		// If the filename() component of the generic - format path contains a period(.), and is not one of the special filesystem elements dot 
		// or dot-dot, then the extension is the substring beginning at the rightmost period (including the period) and until the end of the pathname.
		// If the first character in the filename is a period, that period is ignored (a filename like ".profile" is not treated as an extension)
		// If the pathname is either . or .., or if filename() does not contain the . character, then empty path is returned.
		bool has_extension_internal(size_t& last_dot) const
		{
			last_dot = m_path_string.find_last_of(".");

			if (last_dot != m_path_string.npos && // If there is a dot
				last_dot > 0 && // The dot is not at the beginning of the path
				m_path_string[last_dot - 1] != '/' && // And the previous character is neither a / nor a . (special characters)
				m_path_string[last_dot - 1] != '.')
			{
				size_t lastSeparator = m_path_string.find_last_of("/");

				if (lastSeparator != m_path_string.npos)
				{
					return last_dot > lastSeparator;
				}
				else
				{
					return true;
				}
			}
			else
			{
				return false;
			}
		}

		void add_trailing_separator()
		{
			// Need to check for empty or back will return invalid memory
			if (!m_path_string.empty())
			{
				char lastCharacter = m_path_string.back();

				if (lastCharacter != '/')
				{
					m_path_string += "/";
				}
			}
		}

		void normalize()
		{
			m_path_string.replace_all('\\', '/');
		}

		StringInterface m_path_string;
	};
}