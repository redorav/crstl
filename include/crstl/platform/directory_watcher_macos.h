#pragma once

#include "CoreFoundation/CoreFoundation.h"
#include "CoreServices/CoreServices.h"

crstl_module_export namespace crstl
{
    void fscallback
    (
        ConstFSEventStreamRef streamRef,
        void *clientCallBackInfo,
        size_t numEvents,
        void *eventPaths,
        const FSEventStreamEventFlags eventFlags[],
        const FSEventStreamEventId eventIds[]
    )
    {
        (void)streamRef;
        (void)clientCallBackInfo;
        
        // Cast eventPaths to an array of C strings
        char **paths = (char **)eventPaths;

        //CFArrayRef
        
        // Iterate through the events
        for (size_t i = 0; i < numEvents; i++)
        {
            printf("Event ID: %llu\n", eventIds[i]);
            printf("Path: %s\n", paths[i]);

            // Check event flags to determine the type of event
            FSEventStreamEventFlags flags = eventFlags[i];
            
            if (flags & kFSEventStreamEventFlagItemCreated)
            {
                printf(" - Item created\n");
            }
            
            if (flags & kFSEventStreamEventFlagItemRemoved)
            {
                printf(" - Item removed\n");
            }
            
            if (flags & kFSEventStreamEventFlagItemRenamed)
            {
                printf(" - Item renamed\n");
            }
            
            if (flags & kFSEventStreamEventFlagItemModified)
            {
                printf(" - Item modified\n");
            }
            printf("\n");
        }
    }

	class directory_watcher : public directory_watcher_base
	{
	public:

		directory_watcher()
		{}

		directory_watcher(const char* directory_path, const directory_watcher_parameters& parameters)
		{
            (void)parameters;
            CFStringRef cfpath = CFStringCreateWithCString(NULL, directory_path, kCFStringEncodingUTF8);
            
            CFArrayRef pathsToWatch = CFArrayCreate(NULL, (const void **)&cfpath, 1, nullptr);
            
            CFAbsoluteTime latency = 100000000000.0; // Latency in seconds for event delivery
            
            m_stream = FSEventStreamCreate
            (
                NULL,                     // Allocator
                &fscallback,      // Callback function
                nullptr,
                pathsToWatch,             // Paths to watch
                kFSEventStreamEventIdSinceNow, // Start with events from now
                latency,                  // Latency for event delivery
                kFSEventStreamCreateFlagFileEvents // Enable file-level events
             );
            
            dispatch_queue_t queue = dispatch_queue_create("crstl.fsevents", NULL);
            
            FSEventStreamSetDispatchQueue(m_stream, queue);
            
            if (!FSEventStreamStart(m_stream))
            {
                //fprintf(stderr, "Failed to start stream\n");
                CFRelease(pathsToWatch);
                return;
            }
            
            	
		}

		~directory_watcher()
		{
            //FSEventStreamStop(stream);
            //FSEventStreamInvalidate(stream);
            //FSEventStreamRelease(stream);
            //CFRelease(pathsToWatch);
		}

		bool is_watching() const
		{
            return true;
		}

		void stop()
		{
			
		}

		template<typename DirectoryUpdateCallback>
		void update_sync(const DirectoryUpdateCallback& callback)
		{
            (void)callback;
            FSEventStreamFlushSync(m_stream);
		}

	private:
        
        FSEventStreamRef m_stream;
	};
}
