#pragma once

#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <fcntl.h>
#include <errno.h>

#if defined(CRSTL_OS_WINDOWS)

// corecrt_share.h
#define CRSTL_SH_DENYRW 0x10    // deny read/write mode
#define CRSTL_SH_DENYWR 0x20    // deny write mode
#define CRSTL_SH_DENYRD 0x30    // deny read mode
#define CRSTL_SH_DENYNO 0x40    // deny none mode
#define CRSTL_SH_SECURE 0x80    // secure mode

// stat.h
#define CRSTL_S_IFMT   0xF000 // File type mask
#define CRSTL_S_IFDIR  0x4000 // Directory
#define CRSTL_S_IFCHR  0x2000 // Character special
#define CRSTL_S_IFIFO  0x1000 // Pipe
#define CRSTL_S_IFREG  0x8000 // Regular
#define CRSTL_S_IREAD  0x0100 // Read permission, owner
#define CRSTL_S_IWRITE 0x0080 // Write permission, owner
#define CRSTL_S_IEXEC  0x0040 // Execute/search permission, owner

typedef int errno_t;

extern "C"
{
	__declspec(dllimport) errno_t _sopen_s(int* _FileHandle, char const* _FileName, int _OpenFlag, int _ShareFlag, int _PermissionMode);

	__declspec(dllimport) int _read(int _FileHandle, void* _DstBuf, unsigned int _MaxCharCount);

	__declspec(dllimport) int _write(int _FileHandle, void const* _Buf, unsigned int _MaxCharCount);

	__declspec(dllimport) int _close(int _FileHandle);

	__declspec(dllimport) long _lseek(int _FileHandle, long _Offset, int _Origin);

	__declspec(dllimport) int _unlink(char const* _FileName);

	__declspec(dllimport) int _access(char const* _FileName, int _AccessMode);

	__declspec(dllimport) int _mkdir(char const* _Path);
}

typedef long off_t;

template<size_t Index> struct ssize_t_definer {};
template<> struct ssize_t_definer<4> { typedef long type; };
template<> struct ssize_t_definer<8> { typedef int64_t type; };
typedef ssize_t_definer<sizeof(void*)>::type ssize_t;

namespace crstl
{
	namespace detail
	{
		inline int open(const char* file_path, int open_flags)
		{
			int file_handle = -1;

			int sharing_mode = CRSTL_SH_DENYRW;
			int permission_mode = (open_flags & (O_WRONLY | O_RDWR)) ? CRSTL_S_IWRITE : CRSTL_S_IREAD;

			_sopen_s(&file_handle, file_path, open_flags, sharing_mode, permission_mode);

			return file_handle;
		}

		inline ssize_t read(int file_handle, void* dst_buf, size_t max_char_count)
		{
			return _read(file_handle, dst_buf, (unsigned int)max_char_count);
		}

		inline ssize_t write(int file_handle, const void* buf, size_t max_char_count)
		{
			return _write(file_handle, buf, (unsigned int)max_char_count);
		}

		inline int close(int file_handle)
		{
			return _close(file_handle);
		}

		inline off_t lseek(int file_handle, off_t offset, int origin)
		{
			return _lseek(file_handle, offset, origin);
		}

		inline int unlink(const char* path)
		{
			return _unlink(path);
		}

		inline int access(const char* path, int mode)
		{
			return _access(path, mode);
		}

		inline int mkdir(const char* path)
		{
			return _mkdir(path);
		}
	}
}

#else

#include <unistd.h>
#include <sys/stat.h>

#if defined(CRSTL_OS_LINUX)
#include <sys/sendfile.h>
#endif

namespace crstl
{
	namespace detail
	{
		inline int open(const char* file_path, int open_flags)
		{
			return ::open(file_path, open_flags);
		}

		inline ssize_t read(int file_handle, void* dst_buf, size_t max_char_count)
		{
			return ::read(file_handle, dst_buf, max_char_count);
		}

		inline ssize_t write(int file_handle, const void* buf, size_t max_char_count)
		{
			return ::write(file_handle, buf, max_char_count);
		}

		inline int close(int file_handle)
		{
			return ::close(file_handle);
		}

		inline off_t lseek(int file_handle, off_t offset, int origin)
		{
			return ::lseek(file_handle, offset, origin);
		}

		inline int unlink(const char* path)
		{
			return ::unlink(path);
		}

		inline int access(const char* path, int mode)
		{
			return ::access(path, mode);
		}

		inline int mkdir(const char* path)
		{
			return ::mkdir(path, 0777);
		}
	}
}

#endif

namespace crstl
{
	namespace detail
	{
		inline ssize_t copy(int src_file_handle, int dst_file_handle)
		{
#if defined(CRSTL_OS_LINUX)
			struct stat s_stat = {};
			off_t total_bytes_copied = 0;
			fstat(src_file_handle, &s_stat);
			return sendfile(dst_file_handle, src_file_handle, &total_bytes_copied, s_stat.st_size);
#else
			static const size_t tmp_data_size = 4096;
			char tmp_data[tmp_data_size];

			ssize_t total_bytes_copied = 0;

			while (true)
			{
				ssize_t read_bytes = read(src_file_handle, tmp_data, sizeof(tmp_data));

				if (read_bytes > 0)
				{
					write(dst_file_handle, tmp_data, read_bytes);
					total_bytes_copied += read_bytes;
				}
				else
				{
					break;
				}
			}

			return total_bytes_copied;
#endif
		}
	}
}

#define CRSTL_SEEK_SET 0
#define CRSTL_SEEK_CUR 1
#define CRSTL_SEEK_END 2

crstl_module_export namespace crstl
{
	class file
	{
	public:

		file(const char* file_path, file_flags::t open_flags)
			: m_file_handle(-1)
			, m_file_open_flags(open_flags)
		{
			int open_flags_posix = 0;

			if ((open_flags & file_flags::read) && (open_flags & file_flags::write))
			{
				open_flags_posix |= O_RDWR;
			}
			else if (open_flags & file_flags::write)
			{
				open_flags_posix |= O_WRONLY;
			}
			else if (open_flags & file_flags::read)
			{
				open_flags_posix |= O_RDONLY;
			}

			if (open_flags & file_flags::append)
			{
				open_flags_posix |= O_APPEND;
			}

			if (open_flags & file_flags::create)
			{
				open_flags_posix |= O_CREAT;
			}

			m_file_handle = detail::open(file_path, open_flags_posix);

			crstl_assert(is_open());

			//stat stat_struct;
			//fstat(m_file_handle, )
			//fseeko(m_file, 0, SEEK_END);
			//m_fileSize = _ftelli64(m_file);

			//if (!(open_flags & file_flags::append))
			//{
			//	::rewind(m_file_handle);
			//}
		}

		~file()
		{
			close();
		}

		bool is_open() const
		{
			return m_file_handle != -1;
		}

		size_t read(void* memory, size_t bytes)
		{
			crstl_assert(is_open());
			crstl_assert(m_file_open_flags & file_flags::read);

			ssize_t bytes_read = detail::read(m_file_handle, memory, (unsigned int)bytes);
			crstl_assert(bytes_read >= 0);

			return (size_t)bytes_read;
		}

		size_t write(const void* memory, size_t bytes)
		{
			crstl_assert(is_open());
			crstl_assert(m_file_open_flags & file_flags::write);

			ssize_t bytes_written = detail::write(m_file_handle, memory, (unsigned int)bytes);
			crstl_assert(bytes_written >= 0);

			return (size_t)bytes_written;
		}

		void close()
		{
			if (is_open())
			{
				detail::close(m_file_handle);
				m_file_handle = 0;
			}
		}

		void seek(file_seek_origin::t seek_origin, int64_t byte_offset)
		{
			crstl_assert(is_open());

			int fseek_origin = CRSTL_SEEK_SET;

			switch (seek_origin)
			{
			case file_seek_origin::begin:
				fseek_origin = CRSTL_SEEK_SET;
				break;
			case file_seek_origin::current:
				fseek_origin = CRSTL_SEEK_CUR;
				break;
			case file_seek_origin::end:
				fseek_origin = CRSTL_SEEK_END;
				break;
			}

			detail::lseek(m_file_handle, (long)byte_offset, fseek_origin);
		}

		void rewind()
		{
			crstl_assert(is_open());
			detail::lseek(m_file_handle, 0, CRSTL_SEEK_CUR);
		}

	private:

		int m_file_handle;

		file_flags::t m_file_open_flags;
	};

	inline void file_copy(const char* source_file_path, const char* destination_file_path, file_copy_options::t copy_options)
	{
		int source_file = detail::open(source_file_path, O_RDONLY);

		if (source_file != -1)
		{
			int destination_file_flags = O_CREAT | O_WRONLY;

			if (copy_options & file_copy_options::overwrite)
			{
				destination_file_flags |= O_TRUNC;
			}

			int destination_file = detail::open(destination_file_path, destination_file_flags);

			if (destination_file != -1)
			{
				detail::copy(source_file, destination_file);
				detail::close(destination_file);
			}

			detail::close(source_file);
		}
	}

	inline void file_move(const char* source_file_path, const char* destination_file_path)
	{
		int source_file = detail::open(source_file_path, O_RDONLY);

		if (source_file != -1)
		{
			int destination_file_flags = O_CREAT | O_WRONLY;
			int destination_file = detail::open(destination_file_path, destination_file_flags);

			if (destination_file != -1)
			{
				detail::copy(source_file, destination_file);
				detail::close(destination_file);
			}

			detail::close(source_file);

			detail::unlink(source_file_path);
		}
	}

	inline filesystem_return::t file_delete(const char* file_path)
	{
		int result = detail::unlink(file_path);

		if (result == 0)
		{
			return filesystem_return::success;
		}
		else
		{
			int error = errno;

			if (error == EACCES)
			{
				return filesystem_return::error_access_denied;
			}
		}

		return filesystem_return::error;
	}

	// https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c
	inline bool file_exists(const char* file_path)
	{
		int result = detail::access(file_path, 00);
		return result != -1;
	}

	inline bool directory_exists(const char* directory_path)
	{
		int result = detail::access(directory_path, 00);
		return result != -1;
	}

	inline filesystem_return::t create_directory(const char* directory_path)
	{
		int result = detail::mkdir(directory_path);

		if (result != -1)
		{
			return filesystem_return::success;
		}
		else
		{
			int error = errno;

			if (error == EEXIST)
			{
				return filesystem_return::success;
			}
			else
			{
				return filesystem_return::error;
			}
		}
	}

	namespace detail
	{
		inline path compute_temp_path()
		{
			// TODO Make this more complete
			// On POSIX systems, the path may be the one specified in the environment variables TMPDIR, TMP, TEMP, TEMPDIR, 
			// and, if none of them are specified, the path "/tmp" is returned.
			return path("/tmp");
		}
	}
}