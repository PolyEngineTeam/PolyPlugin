#pragma once

#include <pp/PluginsContainer.hpp>

/*
#######################################################################
### Introduction
#######################################################################

PolyPlugin is an intent library (inspired by intents in Android). It
allows for loading plugins (shared libraries) from a given directory or a
plugin from a direct path.

After loading some plugins it's possible to dispatch intents that will 
be forwarded to one of the loaded plugins.



#######################################################################
### Adding new plugins (plugin implementation)
#######################################################################

Plugins are just shared library projects that export one symbol:
	IPlugin* createPolyPlugin()
Plugin project needs to create a class that derives from IPlugin and 
returns its instance from this exported function. There is a helper 
macro that evaluates to the symbol that needs to be exported:
	POLY_PLUGIN_ENTRY

Example plugin:

	//-------------------------------------------------------------------------------------------------------
	class Plugin : public pp::IPlugin
	{
	public:
		void init(std::shared_ptr<pp::Router> router) final
		{
			router->registerReceiver<AddIntent>(getPluginInfo(), 
				[this] (AddIntent intent) { return addIntentReceiver(std::move(intent)); });
		}

		void deinit(std::shared_ptr<pp::Router> router) final { }
		const pp::PluginInfo& getPluginInfo() const final { return { "Calculator", { 1, 0, 0} }; }

	private:
		AddIntent::Result addIntentReceiver(AddIntent intent) { return intent.a + intent.b; }
	};

	//-------------------------------------------------------------------------------------------------------
	POLY_PLUGIN_ENTRY() { return new Plugin(); }

This example shows a plugin that supports processing of 'AddIntent' intent.



#######################################################################
### Adding new intents (plugin API)
#######################################################################

But to process an intent this intent must be defined somewhere. It's 
done in the plugin API. Your plugin can define its own intents or just 
use intents defined by the other plugins and then the user can choose if an
intent should be processed by its original plugin or by your plugin 
with custom handling of this intent.

There are only two requirements that class needs to fulfill so its 
instances can be processed as intents:
	1. Type name nested in this class named "Result" - it's needed to 
		deduce return type of intent receivers.
	2. Static field of type IntentInfo and name "Info" - it's used to 
		check if an intent handler can process given intent (intent 
		receiver must be compiled with the same intent version as a given
		intent.

Example intent:

	//-------------------------------------------------------------------------------------------------------
	class AddIntent
	{
	public:
		using Result = int;
		static inline pp::IntentInfo Info = { "AddIntent", 3 };

		int a = 0;
		int b = 0;
	};

*/