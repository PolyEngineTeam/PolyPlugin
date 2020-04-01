#pragma once

#include <filesystem>
#include <windows.h>

#include <pp/IntentRouter.hpp>

namespace pp
{
	class IPlugin;

	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	// The main class of this library. It allows user to load plugins 
	// from directory or path and get the intent router to dispatch 
	// some intents. It also owns the IPlugin instances whereas 
	// IntentRouter only routes intents to correct handlers.
	class PluginsContainer
	{
	public:
		// As it is the main class of this library it also contains the
		// library version. After loading a plugin the PolyPlugin major
		// version used by the loaded plugin will be compared to this 
		// one and if they differ the plugin will be ignored and 
		// deleted
		static inline Version polyPluginVersion = { 0, 1, 0 };

		// Default ctor. If not provided the IntentRouter will be 
		// created automatically.
		PluginsContainer() : m_intentRouter(std::make_shared<IntentRouter>()) {}
		
		// If the user needs custom intent router it can be provided in
		// this ctor and it will be used instead of the default one
		// @param router - customized router that should be used by 
		//		this container
		PluginsContainer(std::shared_ptr<IntentRouter> router) : m_intentRouter(std::move(router)) {}

		~PluginsContainer();

		// @returns collection of loaded plugins from shared libraries 
		//		existing in the given root directory and possibly all 
		//		recursive subdirectories if 'recursive' flag is enabled.
		//		Loaded plugins are owned by this PluginsContainer and 
		//		are already initialized.
		// @param path - path to directory where to search for shared.
		//		libraries
		// @param recursive - if this param is true then this method will 
		//		return shared libraries from not only given directory but 
		//		also all recursive subdirectories
		std::vector<std::weak_ptr<IPlugin>> load(std::filesystem::path root, bool reccursive);

		// @returns intent router. Returned router is used to 
		//		initialize all plugins that were loaded or will be 
		//		loaded
		const std::shared_ptr<IntentRouter>& getIntentRouter() const { return m_intentRouter; }

	private:
		std::vector<std::shared_ptr<IPlugin>> m_plugins;
		std::shared_ptr<IntentRouter> m_intentRouter;
	}; // class PluginsContainer

	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	// Plugin interface. All projects that want to be loaded as plugins
	// need to override this class and export function from shared 
	// library with name 'createPolyPlugin'. This function must return 
	// pointer to the instance of plugin class (the one derived from 
	// IPlugin). Plugin is instantiated only once, right after its 
	// shared library is loaded. It is deinitialized just before 
	// unloading its shared library.
	class IPlugin
	{
	public:
		virtual ~IPlugin() = default;

		// Initializes the plugin. Called right after library is loaded
		// and plugin is created. This is the place where the library 
		// should register its intent handlers.
		// @param router - plugin should register its intent handlers 
		//		in this router.
		virtual void init(std::shared_ptr<IntentRouter> router) = 0;
		// Called just before the plugin object is deleted and the 
		// whole shared library is unloaded. This is the place for 
		// cleanups and dispatching intents informing desired users 
		// that this plugin is unloaded.
		// @param router - intent handlers registered by this plugin 
		//		are automatically removed so there is no need to remove
		//		them here but also it's not a big problem if you do 
		//		this. The main purpose of this param is to allow plugin
		//		to dispatch messages to other plugins that may be 
		//		concerned about the lifetime of this plugin.
		virtual void deinit(std::shared_ptr<IntentRouter> router) = 0;

		// Plugin name and version are only for display purposes and 
		// might be useful for other IReceiverSelector implementation.
		virtual PluginInfo getPluginInfo() const = 0;

		// This is PolyPlugin version used by this plugin. If major 
		// version differs from one saved in PluginsContainer which 
		// loads this plugin this plugin will be ignored and deleted 
		// even before init. This field is initialized with version of
		// PolyPlugin that was used to compile shared library 
		// implementing this IPlugin interface so the user doesn't need
		// to worry about the value of this field.
		const Version usedPolyPluginVersion = PluginsContainer::polyPluginVersion;
	}; // class IPlugin

	// Helper using statement to simplify usage of type of a function 
	// that is loaded from the shared library and provides instance of
	// IPlugin.
	using PluginCreatorType = IPlugin * (__stdcall *)();

	namespace internal
	{
		//-------------------------------------------------------------------------------------------------------
		// @returns paths to all shared libs in given directory and 
		//		possibly all nested directories.
		// @param path - path to directory where to search for shared 
		//		libraries
		// @param recursive - if this param is true then this method will 
		//		return shared libraries from not only given directory but 
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
						if (std::filesystem::is_regular_file(*it) && it->path().extension() == ".dll")
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
						if (std::filesystem::is_regular_file(*it) && it->path().extension() == ".dll")
							result.push_back(it->path());
						++it;
					}
				}
			}
			else
			{
				if (std::filesystem::is_regular_file(path) && path.extension() == ".dll")
					result.push_back(path);
			}

			return result;
		}

		//-------------------------------------------------------------------------------------------------------
		// @returns collection of loaded plugins from shared libraries 
		//		existing in the given root directory and possibly all 
		//		recursive subdirectories if 'recursive' flag is enabled.
		// @param path - path to directory where to search for shared 
		//		libraries
		// @param recursive - if this param is true then this method will 
		//		return shared libraries from not only given directory but 
		//		also all recursive subdirectories
		std::vector<std::shared_ptr<IPlugin>> loadPlugins(std::filesystem::path root, bool recursive)
		{
			std::vector<std::shared_ptr<IPlugin>> result;

			for (const std::filesystem::path& path : getAllSharedLibs(root, recursive))
			{
				HINSTANCE hGetProcIDDLL = LoadLibrary(LPCSTR(path.string().c_str()));
				if (!hGetProcIDDLL)
					continue;

				PluginCreatorType creator = (PluginCreatorType)GetProcAddress(hGetProcIDDLL, "createPolyPlugin");
				auto w = GetLastError();
				if (!creator)
					continue;

				std::shared_ptr<IPlugin> newPlugin(creator());
				result.push_back(std::move(newPlugin));
			}

			return result;
		}
	}

	//-------------------------------------------------------------------------------------------------------
	PluginsContainer::~PluginsContainer()
	{
		assert(m_intentRouter.use_count() == 1);
		for (const std::shared_ptr<IPlugin>& plugin : m_plugins)
			assert(plugin.use_count() == 1);
	}

	//-------------------------------------------------------------------------------------------------------
	inline std::vector<std::weak_ptr<IPlugin>> pp::PluginsContainer::load(std::filesystem::path root, bool reccursive)
	{
		std::vector<std::weak_ptr<IPlugin>> result;

		for (std::shared_ptr<IPlugin> plugin : internal::loadPlugins(std::move(root), reccursive))
		{
			if (plugin->usedPolyPluginVersion.major == polyPluginVersion.major)
			{
				plugin->init(m_intentRouter);
				result.push_back(plugin);
				m_plugins.push_back(std::move(plugin));
			}
		}

		return result;
	}

} // namespace pp

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
// Helper macro for library users. It is declaration of method that 
// needs to be exported from shared library so PolyPlugin will be 
// able to load IPlugin instance form there.
#define POLY_PLUGIN_ENTRY \
		extern "C" __declspec(dllexport) ::pp::IPlugin* __stdcall createPolyPlugin