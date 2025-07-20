#pragma once

#include <sys/inotify.h>

// TODO Do we need one per thread?
namespace crstl
{
	template<typename Dummy>
	struct inotify_globals
	{
		static const int inotify_fd;
	};

	template<typename Dummy>
	const int inotify_globals<Dummy>::inotify_fd = inotify_init();
}

crstl_module_export namespace crstl
{
	class directory_watcher : public directory_watcher_base
	{
	public:

		directory_watcher()
			: m_watch_id(-1)
		{

		}

		directory_watcher(const char* directory_path, const directory_watcher_parameters& parameters)
			: m_watch_id(-1)
		{
			(void)parameters;
		
			int inotify_fd = inotify_globals<Dummy>::inotify_fd;

			if (inotify_fd != -1)
			{
				m_watch_id = inotify_add_watch(inotify_fd, directory_path, IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO);
			}
		}

		~directory_watcher()
		{
			inotify_rm_watch(inotify_globals<Dummy>::inotify_fd, m_watch_id);

			close(m_watch_id);
		}

		bool is_watching() const
		{
			return m_watch_id != -1;
		}

		void stop()
		{
			close(m_watch_id);
			m_watch_id = -1;
		}

		template<typename DirectoryUpdateCallback>
		void update_sync(const DirectoryUpdateCallback& callback)
		{
			if (is_watching())
			{
				static const size_t buffer_length = 1024 * (sizeof(struct inotify_event) + kMaxPathLength);

				char buffer[buffer_length];

				int length = read(inotify_globals<Dummy>::inotify_fd, buffer, buffer_length);

				if (length > 0)
				{
					int event_index = 0;
					while (event_index < length)
					{
						struct inotify_event* event = (struct inotify_event*)&buffer[event_index];
						
						if (event->len)
						{
							directory_watcher_action watcher_action = directory_watcher_action::none;

							if (event->mask & IN_CREATE)
							{
								watcher_action = directory_watcher_action::file_created;
							}
							else if (event->mask & IN_MODIFY)
							{
								watcher_action = directory_watcher_action::file_modified;
							}
							else if (event->mask & IN_DELETE)
							{
								watcher_action = directory_watcher_action::file_deleted;
							}
							else if (event->mask & IN_MOVED_FROM)
							{
								watcher_action = directory_watcher_action::file_renamed_old_name;
							}
							else if (event->mask & IN_MOVED_TO)
							{
								watcher_action = directory_watcher_action::file_renamed_new_name;
							}

							if (watcher_action != directory_watcher_action::none)
							{
								directory_modified_entry modified_entry;
								modified_entry.action = watcher_action;
								modified_entry.filename = event->name;
								callback(modified_entry);
							}
						}

						event_index += sizeof(struct inotify_event) + event->len;
					}
				}
			}
		}

	private:

		int m_watch_id;
	};
}
