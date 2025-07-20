#pragma once

crstl_module_export namespace crstl
{
	class directory_watcher : public directory_watcher_base
	{
	public:

		directory_watcher()
		{}

		directory_watcher(const char* directory_path, const directory_watcher_parameters& parameters)
		{
			
		}

		~directory_watcher()
		{
			
		}

		bool is_watching() const
		{
			
		}

		void stop()
		{
			
		}

		template<typename DirectoryUpdateCallback>
		void update_sync(const DirectoryUpdateCallback& callback)
		{
			
		}

	private:

	};
}