#pragma once

#include <CmakeConfig.hpp>

#include <pp/Defines.hpp>
#include <pp/Router.hpp>
#include <pp/PluginsLoader.hpp>

namespace pp
{
	class IPlugin;

	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	// The main class of this library. It allows the user to load plugins 
	// from a directory or a path and get the intent router to dispatch 
	// some intents. It also owns the IPlugin instances whereas 
	// Router only routes intents to correct handlers.
	class PluginsContainer
	{
	public:
		// As it is the main class of this library it also contains the
		// library version. After loading a plugin the PolyPlugin major
		// version used by the loaded plugin will be compared to this 
		// one and if they differ the plugin will be ignored and 
		// deleted
		static inline Version polyPluginVersion = { PROJECT_VER_MAJOR, PROJECT_VER_MINOR, PROJECT_VER_PATCH };

		// Default ctor. If not provided the Router will be 
		// created automatically.
		PluginsContainer() : m_Router(std::make_shared<Router>()) {}
		
		// If the user needs a custom intent router it can be provided in
		// this ctor and it will be used instead of the default one
		// @param router - customized router that should be used by 
		//		this container
		PluginsContainer(std::shared_ptr<Router> router) : m_Router(std::move(router)) {}

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
		std::vector<std::weak_ptr<IPlugin>> load(std::filesystem::path root, bool recursive);

		// @returns intent router. Returned router is used to 
		//		initialize all plugins that were loaded or will be 
		//		loaded
		const std::shared_ptr<Router>& getRouter() const { return m_Router; }

	private:
		std::vector<std::shared_ptr<IPlugin>> m_plugins;
		std::shared_ptr<Router> m_Router;
	}; // class PluginsContainer

	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	// Plugin interface. All projects that want to be loaded as plugins
	// need to override this class and export the function from shared 
	// library named 'createPolyPlugin'. This function must return a
	// pointer to the instance of plugin class (the one derived from 
	// IPlugin). Plugin is instantiated only once, right after its 
	// shared library is loaded. It is deinitialized just before 
	// unloading its shared library.
	class IPlugin
	{
	public:
		virtual ~IPlugin() = default;

		// Initializes the plugin. Called right after the library is loaded
		// and the plugin is created. This is the place where the library 
		// should register its intent handlers.
		// @param router - plugin should register its intent handlers 
		//		in this router.
		virtual void init(std::shared_ptr<Router> router) = 0;
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
		virtual void deinit(std::shared_ptr<Router> router) = 0;

		// Plugin name and version are only for display purposes and 
		// might be useful for other ISelector implementation.
		virtual PluginInfo getPluginInfo() const = 0;

		// This is the PolyPlugin version used by this plugin. If the major 
		// version differs from the one saved in PluginsContainer which 
		// loads this plugin this plugin will be ignored and deleted 
		// even before init. This field is initialized with version of
		// PolyPlugin that was used to compile shared library 
		// implementing this IPlugin interface so the user doesn't need
		// to worry about the value of this field.
		const Version usedPolyPluginVersion = PluginsContainer::polyPluginVersion;
	}; // class IPlugin

	//-------------------------------------------------------------------------------------------------------
	PluginsContainer::~PluginsContainer()
	{
		assert(m_Router.use_count() == 1);
		for (const std::shared_ptr<IPlugin>& plugin : m_plugins)
			assert(plugin.use_count() == 1);
	}

	//-------------------------------------------------------------------------------------------------------
	inline std::vector<std::weak_ptr<IPlugin>> pp::PluginsContainer::load(std::filesystem::path root, bool recursive)
	{
		std::vector<std::weak_ptr<IPlugin>> result;

		for (std::shared_ptr<IPlugin> plugin : PluginsLoader::loadPlugins(std::move(root), recursive))
		{
			if (plugin->usedPolyPluginVersion.major == polyPluginVersion.major)
			{
				plugin->init(m_Router);
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
		extern "C" PP_EXPORT ::pp::IPlugin* STDCALL createPolyPlugin
