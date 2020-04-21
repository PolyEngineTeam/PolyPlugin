#include <iostream>

#include <pp/PolyPlugin.hpp>

#include <AddIntent.hpp>

//-------------------------------------------------------------------------------------------------------
class Plugin : public pp::IPlugin
{
public:
	void init(std::shared_ptr<pp::IntentRouter> router) final
	{
		router->registerReceiver<AddIntent>(getPluginInfo(), [this] (AddIntent intent) { return addIntentReceiver(std::move(intent)); });
	}

	void deinit(std::shared_ptr<pp::IntentRouter> router) final { }
	pp::PluginInfo getPluginInfo() const final { return { "Calculator", { 1, 0, 0} }; }

private:
	AddIntent::Result addIntentReceiver(AddIntent intent) { return intent.a + intent.b; }
};

//-------------------------------------------------------------------------------------------------------
POLY_PLUGIN_ENTRY()
{
	return new Plugin();
}