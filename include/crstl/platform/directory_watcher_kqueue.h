#pragma once

#include <sys/event.h>

#include <stdio.h> // TODO DELETE

#include "crstl/fixed_path.h"
#include "crstl/vector.h"

crstl_module_export namespace crstl
{
	class directory_watcher : public directory_watcher_base
	{
	public:

		directory_watcher()
            : m_directory_descriptor(-1)
            , m_kqueue(-1)
            , m_file_descriptor_count(0)
		{}

		directory_watcher(const char* directory_path, const directory_watcher_parameters& parameters) : directory_watcher()
		{
            (void)parameters;
            
            m_file_events.reserve(1024);
            m_file_names.reserve(1024);
            
            m_directory_descriptor = open(directory_path, O_EVTONLY);

            if(m_directory_descriptor != -1)
            {
                m_file_events.push_back();
                m_file_names.push_back();
                
                EV_SET(&m_file_events[0], m_directory_descriptor, EVFILT_VNODE,
                    EV_ADD |    // Add event to the queue
                    EV_ENABLE | // Enable notifications
                    EV_CLEAR,   // Clear when processed
                    NOTE_WRITE |
                    NOTE_EXTEND |
                    NOTE_ATTRIB |
                    NOTE_LINK |
                    NOTE_RENAME |
                    NOTE_DELETE,
                    0, nullptr);
                
                m_file_names[0] = directory_path;
                
                m_kqueue = kqueue();
                
                if(m_kqueue != -1)
                {
                    DIR* dirp = opendir(directory_path);
                    
                    // Now that we've started monitoring the directory, add all existing files to be monitored
                    struct dirent* entry;
                    while ((entry = readdir(dirp)))
                    {
                        if (entry->d_type == DT_REG) // Regular file
                        {
                            track_file(directory_path, entry->d_name);
                        }
                    }
                }
                else
                {
                    close(m_directory_descriptor);
                    m_directory_descriptor = -1;
                }
            }
		}

		~directory_watcher()
		{
            
		}

		bool is_watching() const
		{
            return true;
		}

		void stop()
		{
			
		}
        
        void track_file(const char* directory_path, const char* filename)
        {
            // Build full path
            crstl::fixed_path1024 filepath;
            
            filepath = directory_path;
            filepath /= filename;
            
            // Find valid file descriptor
            int file_descriptor = open(filepath.c_str(), O_EVTONLY);
            if (file_descriptor == -1)
            {
                return;
            }
            
            struct kevent& monitored_event = m_file_events.push_back();

            EV_SET
            (
                &monitored_event, file_descriptor, EVFILT_VNODE,
                EV_ADD | EV_ENABLE | EV_CLEAR,
                NOTE_WRITE | NOTE_DELETE | NOTE_EXTEND | NOTE_RENAME,
                0, NULL
            );
            
            m_file_names.push_back() = filename;
            m_file_descriptor_count++;
        }
        
        void untrack_file(int file_descriptor)
        {
            (void)file_descriptor;
        }
        
        size_t find_event_index(uintptr_t file_descriptor)
        {
            for(size_t i = 0; i < m_file_events.size(); ++i)
            {
                if(m_file_events[i].ident == file_descriptor)
                {
                    return i;
                }
            }
            
            return 0xffffffff;
        }

		template<typename DirectoryUpdateCallback>
		void update_sync(const DirectoryUpdateCallback& callback)
		{
            (void)callback;
            
            struct timespec timeout;
            timeout.tv_sec = 1;
            timeout.tv_nsec = 0;
            
            struct kevent received_changes[1024 + 1];
            int file_event_count = kevent(m_kqueue, m_file_events.data(), m_file_descriptor_count + 1, received_changes, 1024, &timeout);
            
            if(file_event_count < 0)
            {
                int the_error = *__error();
                printf("There was an error %i", the_error);
            }
            else if(file_event_count > 0)
            {
                for(int i = 0; i < file_event_count; ++i)
                {
                    struct kevent& event = received_changes[i];
                    
                    crstl::fixed_path1024 file_path;
                    
                    char file_path_cstr[PATH_MAX];
                    if (fcntl((int)event.ident, F_GETPATH, file_path_cstr) != -1)
                    {
                        file_path = file_path_cstr;
                    }
                    
                    printf("%s fd %lu changed\n", file_path.c_str(), event.ident);
                    
                    size_t position = find_event_index(event.ident);
                    
                    // Assume we have the correct filename unless it's been renamed
                    if (event.fflags & NOTE_RENAME)
                    {
                        printf("NOTE_RENAME\n");
                        
                        const crstl::fixed_path1024& original_path = m_file_names[position];
                        
        #if defined(CRSTL_OS_MACOS)
                        // On MacOS, if we delete a file it ends up being "renamed" by being sent to the rubbish bin. As far as we're concerned, the file has been deleted
                        // and we should report it that way. For every rename we need to try to find if the directory contain .Trash.
                        if(file_path.find(".Trash") != file_path.npos)
                        {
                            // Call
                            crstl::directory_modified_entry entry_deleted;
                            entry_deleted.action = crstl::directory_watcher_action::file_deleted;
                            entry_deleted.filename = original_path.c_str();
                            callback(entry_deleted);
                        }
                        else
        #endif
                        {
                            // Call callback with the old name
                            crstl::directory_modified_entry entry_old_name;
                            entry_old_name.action = crstl::directory_watcher_action::file_renamed_old_name;
                            entry_old_name.filename = m_file_names[position].c_str();
                            callback(entry_old_name);
                            
                            // Call callback with the new name
                            crstl::directory_modified_entry entry_new_name;
                            entry_new_name.action = crstl::directory_watcher_action::file_renamed_new_name;
                            entry_new_name.filename = file_path.c_str();
                            callback(entry_new_name);
                            
                            // Update path now that it's been renamed
                            m_file_names[position] = file_path.filename().c_str();
                        }
                        
                        //watcher_action = directory_watcher_action::file_deleted;
                    }
                    
                    if (event.fflags & NOTE_ATTRIB)
                    {
                        printf("NOTE_ATTRIB\n");
                        //watcher_action = directory_watcher_action::file_created;
                    }
                    //else if (event.fflags & NOTE_CLOSE)
                    //{
                    //    printf("NOTE_CLOSE\n");
                    //    //watcher_action = directory_watcher_action::file_modified;
                    //}
                    //else if (event.fflags & NOTE_CLOSE_WRITE)
                    //{
                    //    printf("NOTE_CLOSE_WRITE\n");
                    //    //watcher_action = directory_watcher_action::file_modified;
                    //}
                    if (event.fflags & NOTE_DELETE)
                    {
                        printf("NOTE_DELETE\n");
                        //watcher_action = directory_watcher_action::file_deleted;
                    }
                    if (event.fflags & NOTE_EXTEND)
                    {
                        printf("NOTE_EXTEND\n");
                        //watcher_action = directory_watcher_action::file_modified;
                    }
                    if (event.fflags & NOTE_LINK)
                    {
                        printf("NOTE_LINK\n");
                        //watcher_action = directory_watcher_action::file_deleted;
                    }
                    //else if (event.fflags & NOTE_OPEN)
                    //{
                    //    printf("NOTE_OPEN\n");
                    //    //watcher_action = directory_watcher_action::file_deleted;
                    //}
                    
                    if (event.fflags & NOTE_REVOKE)
                    {
                        printf("NOTE_REVOKE\n");
                        //watcher_action = directory_watcher_action::file_deleted;
                    }
                    
                    if (event.fflags & NOTE_WRITE)
                    {
                        printf("NOTE_WRITE\n");
                        //watcher_action = directory_watcher_action::file_modified;
                    }
                }
            }
		}

	private:
        
        int m_directory_descriptor;
        
        crstl::fixed_path1024 m_directory_path;
        
        int m_kqueue;
        
        int m_file_descriptor_count;
        
        // All events
        crstl::vector<struct kevent> m_file_events;
        
        // Only track the filenames as we know what the directory is
        crstl::vector<crstl::fixed_string256> m_file_names;
	};
}
