#pragma once

#include <filesystem>

#include <pp/Defines.hpp>
#include <pp/PluginWrapper.hpp>

namespace pp
{
	namespace PluginsLoader
	{
		//-------------------------------------------------------------------------------------------------------
		// @returns true if it is a plugin type, false otherwise.
		// @param path - path to the file being checked
		bool isPlugin(const std::filesystem::path& path)
		{
		#if defined(_WIN32)
			return path.extension() == ".dll";
		#elif defined(__APPLE__)
			return path.extension() == ".dylib";
		#else
			return path.extension() == ".so";
		#endif
		}

		//-------------------------------------------------------------------------------------------------------
		// @returns paths to all shared libs in a given directory and 
		//		possibly all nested directories.
		// @param path - path to directory where to search for shared 
		//		libraries
		// @param recursive - if this param is true then this method will 
		//		return shared libraries from not only the given directory but 
		//		also all recursive subdirectories
		std::vector<std::filesystem::path> getAllSharedLibs(std::filesystem::path path, bool recursive)
		{
			if (!std::filesystem::exists(path))
				return {};

			std::vector<std::filesystem::path> result;

			if (std::filesystem::is_directory(path))
			{
				if (recursive)
				{
					std::filesystem::recursive_directory_iterator it(path);
					std::filesystem::recursive_directory_iterator endit;

					while (it != endit)
					{
						if (std::filesystem::is_regular_file(*it) && isPlugin(it->path()))
							result.push_back(it->path());
						++it;
					}
				}
				else
				{
					std::filesystem::directory_iterator it(path);
					std::filesystem::directory_iterator endit;

					while (it != endit)
					{
						if (std::filesystem::is_regular_file(*it) && isPlugin(it->path()))
							result.push_back(it->path());
						++it;
					}
				}
			}
			else
			{
				if (std::filesystem::is_regular_file(path) && isPlugin(path))
					result.push_back(path);
			}

			return result;
		}

		//-------------------------------------------------------------------------------------------------------
		// @returns collection of loaded plugins from shared libraries 
		//		existing in the given root directory and possibly all 
		//		recursive subdirectories if 'recursive' flag is enabled.
		// @param path - path to the directory where to search for shared 
		//		libraries
		// @param recursive - if this param is true then this method will 
		//		return shared libraries from not only the given directory but 
		//		also all recursive subdirectories
		std::vector<std::shared_ptr<PluginWrapper>> loadPlugins(std::filesystem::path root, bool recursive)
		{
			std::vector<std::shared_ptr<PluginWrapper>> result;

			for (const std::filesystem::path& path : getAllSharedLibs(root, recursive))
			{
				PluginWrapper wrapper = PluginWrapper::loadPluginEntryPoint(path);
				if (!wrapper.isValid())
					continue;

				wrapper();
				result.push_back(std::make_shared<PluginWrapper>(std::move(wrapper)));
			}

			return result;
		}
	}
}