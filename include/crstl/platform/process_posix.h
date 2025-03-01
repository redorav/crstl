#pragma once

#include "crstl/utility/string_length.h"

#include <signal.h>
#include <stdio.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

// https://www.gnu.org/software/gnulib/manual/html_node/environ.html
#if defined(CRSTL_OS_OSX)
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#endif

crstl_module_export namespace crstl
{
	class process : public process_base
	{
	public:

		process() 
			: process_base()
			, m_child_pid(0)
			, m_stdout_read_file(nullptr)
		{}

		process(const char* executable, const char* args) 
			: process_base()
			, m_child_pid(0)
			, m_stdout_read_file(nullptr)
		{
			int stdoutfd[2];
			if (pipe(stdoutfd) != 0)
			{
				m_state = process_state::error_failed_to_launch;
				return;
			}

			posix_spawn_file_actions_t actions;
			if (posix_spawn_file_actions_init(&actions) != 0)
			{
				m_state = process_state::error_failed_to_launch;
				return;
			}

			// Close the stdout read end and map the write end to stdout
			if (posix_spawn_file_actions_addclose(&actions, stdoutfd[0]) != 0 ||
				posix_spawn_file_actions_adddup2(&actions, stdoutfd[1], STDOUT_FILENO) != 0)
			{
				m_state = process_state::error_failed_to_launch;
			}

			if (m_state == process_state::undefined)
			{
				char* argv[] = { (char*)args, nullptr };
				int spawn_return = posix_spawn(&m_child_pid, executable, &actions, nullptr/*attrp*/, argv, environ/*envp*/);

				if (spawn_return == 0)
				{
					m_state = process_state::launched;
				}
				else
				{
					m_state = process_state::error_failed_to_launch;
				}
			}

			// Close the write end of stdout as we'll only read it
			close(stdoutfd[1]);

			// Open the stdout as a readonly binary file
			m_stdout_read_file = fdopen(stdoutfd[0], "rb");

			posix_spawn_file_actions_destroy(&actions);
		}

		~process()
		{
			if (m_stdout_read_file)
			{
				fclose(m_stdout_read_file);
			}
		}

		process(process&& other)
		{
			m_child_pid = other.m_child_pid;
			m_state = other.m_state;

			other.m_child_pid = 0;
			other.m_state = process_state::undefined;
		}

		process& operator = (process&& other)
		{
			terminate();

			m_child_pid = other.m_child_pid;
			m_state = other.m_state;

			other.m_child_pid = 0;
			other.m_state = process_state::undefined;

			return *this;
		}

		bool is_running() const
		{
			// https://linux.die.net/man/2/waitpid
			// On success, returns the process ID of the child whose state has changed; 
			// if WNOHANG was specified and one or more child(ren) specified by pid exist, 
			// but have not yet changed state, then 0 is returned. On error, -1 is returned.
			int status;
			int wpid = waitpid(m_child_pid, &status, WNOHANG);
			return wpid == 0;
		}

		process_exit_code wait()
		{
			if (m_state == process_state::launched)
			{
				int status;
				int wpid = waitpid(m_child_pid, &status, 0);

				if (m_child_pid == wpid)
				{
					if (WIFEXITED(status))
					{
						int return_value = WEXITSTATUS(status);
						m_state = process_state::waited;
						m_exit_code = process_exit_code((process_exit_code::t)return_value);
					}
					else
					{
						m_state = process_state::error_wait;
					}
				}
				else
				{
					m_state = process_state::error_wait;
				}

				m_child_pid = 0;
			}

			return m_exit_code;
		}

		process_size read_stdout(char* buffer, size_t buffer_size)
		{
			crstl_assert_msg(buffer != nullptr, "Buffer is null");
			crstl_assert_msg(buffer_size > 0, "Invalid size");
			
			ssize_t bytes_read = 0;

			// We need to check whether the process is alive because it might have been killed by someone else
			// before we're trying to access the stdout handle. There's not a lot we can do 
			if ((m_state == process_state::launched || m_state == process_state::waited) && is_alive())
			{
				int fd = fileno(m_stdout_read_file);
				bytes_read = read(fd, buffer, buffer_size);

				if (bytes_read >= 0)
				{
					return process_size(bytes_read);
				}
			}

			return process_size();
		}

		bool terminate()
		{
			if (m_state == process_state::launched)
			{
				int result = kill(m_child_pid, 9);
				m_state = process_state::terminated;
				return result;
			}
			
			return false;
		}

	private:

		pid_t m_child_pid;

		FILE* m_stdout_read_file;
	};
};