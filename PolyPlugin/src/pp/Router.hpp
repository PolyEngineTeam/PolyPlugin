#pragma once

#include <memory>
#include <optional>
#include <map>
#include <cassert>
#include <numeric>

#include <pp/FunctionsCollection.hpp>

namespace pp
{
	class Selector;

	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	// This class' purpose is to allow registering/unregistering intent
	// handlers and provide interface for intent dispatching.
	class Router
	{
	public:
		// Create intent router with default handler selector which 
		// will always select the first registered intent handler.
		Router() : m_selector(std::make_shared<Selector>()) {}

		// Allows user to provide custom handler selector.
		// @parem selector - handler selector provided by user
		Router(std::shared_ptr<Selector> selector) : m_selector(std::move(selector)) {}

		// Registers intent handler in this router. If someone
		// will dispatch intent with type matching given handler with 
		// 'processIntent' method the registered handler will be sent 
		// to the selector along with the other handlers matching 
		// intent type and selector will choose which handler should 
		// process the intent.
		// @tparam T - type of an intent which can be processed by the given
		//		'handler'
		// @param info - plugin info is not necessary for this class to
		//		work but it might be useful to be able to present to 
		//		the user names and versions of plugins that are able to 
		//		process the intent if there are multiple intent 
		//		handlers that match the intent type.
		// @param handler - intent handler to register
		template <typename T>
		void registerIntentHandler(PluginInfo info, std::function<typename T::Result(T)> handler);

		template <typename T>
		void registerEventReceiver(PluginInfo info, std::function<typename T::Result(const T&)> receiver);

		// This method is used for intents dispatching. When it's 
		// called it asks handler selector which intent handler 
		// should be used to process given intent.
		// @tparem T - type of the intent that needs to be processed
		// @returns empty optional if handler for given intent wasn't 
		//		found or IntentType::Result returned from intent 
		//		handler.
		// @param intent - intent that needs to be processed
		template <typename T>
		std::optional<typename T::Result> processIntent(T intent);

		template <typename T>
		std::vector<std::optional<typename T::Result>> processEvent(const T& event);

	private:
		std::shared_ptr<Selector> m_selector;

		// Handlers are mapped by intent info so older intents (with 
		// older version) also can be processed if handler with 
		// matching intent type is registered in this intent router.
		std::map<IntentInfo, std::unique_ptr<HandlersCollectionBase>> m_handlers;
		std::map<EventInfo, std::unique_ptr<ReceiversCollectionBase>> m_receivers;
	}; // class Router

	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	// Helper class for intent router. This class is meant to be 
	// overridden by the user so it will be possible to implement 
	// choosing the default intent handler (like in android when some
	// action is taken for the first time and there are more than one 
	// plugins that can handle this intent, for example opening photo 
	// gallery to select a picture).
	class Selector
	{
	public:
		virtual ~Selector() = default;

		// This method is called every time intent router has to 
		// process the intent.Default implementation chooses the first
		// handler (the first that was registered for this type of 
		// intent).
		// @returns index of the handler present in the given 
		//		'handlers' collection that should process the intent
		//		with given 'intentInfo'
		// @param intent - intent info of an intent that needs to be 
		//		processed
		// @param handlers - collection of available handlers
		virtual int selectHandler(IntentInfo intent, std::vector<PluginInfo> handlers) { return 0; }

		virtual std::vector<int> selectReceivers(EventInfo event, std::vector<PluginInfo> receivers)
		{
			std::vector<int> result(receivers.size());
			std::iota(result.begin(), result.end(), 0);
			return result;
		}
	}; // class Selector

	//-------------------------------------------------------------------------------------------------------
	template<typename T>
	inline void Router::registerIntentHandler(PluginInfo info, std::function<typename T::Result(T)> handler)
	{
		const auto it = m_handlers.find(T::Info);
		if (it != m_handlers.end())
			static_cast<HandlersCollection<T>*>(it->second.get())->push_back({ std::move(info), std::move(handler) });
		else
		{
			auto newCollection = std::make_unique<HandlersCollection<T>>();
			newCollection->push_back({ std::move(info), std::move(handler) });
			m_handlers.insert({ T::Info, std::move(newCollection) });
		}
	}

	//-------------------------------------------------------------------------------------------------------
	template<typename T>
	inline void Router::registerEventReceiver(PluginInfo info, std::function<typename T::Result(const T&)> receiver)
	{
		const auto it = m_receivers.find(T::Info);
		if (it != m_receivers.end())
			static_cast<ReceiversCollection<T>*>(it->second.get())->push_back({ std::move(info), std::move(handler) });
		else
		{
			auto newCollection = std::make_unique<ReceiversCollection<T>>();
			newCollection->push_back({ std::move(info), std::move(handler) });
			m_receivers.insert({ T::Info, std::move(newCollection) });
		}
	}

	//-------------------------------------------------------------------------------------------------------
	template<typename T>
	inline std::optional<typename T::Result> Router::processIntent(T intent)
	{
		const auto it = m_handlers.find(T::Info);
		if (it != m_handlers.end())
		{
			const auto handlersCollection = static_cast<HandlersCollection<T>*>(it->second.get());
			const int chosenHandler = m_selector->selectHandler(T::Info, handlersCollection->getPluginsInfo());
			return handlersCollection->at(chosenHandler).second(std::move(intent));
		}
		else
			return {};
	}

	//-------------------------------------------------------------------------------------------------------
	template<typename T>
	inline std::vector<std::optional<typename T::Result>> Router::processEvent(const T& event)
	{
		const auto it = m_receivers.find(T::Info);
		if (it != m_receivers.end())
		{
			const auto receiversCollection = static_cast<ReceiversCollection<T>*>(it->second.get());
			const std::vector<int> chosenReceivers = m_selector->selectReceivers(T::Info, receiversCollection->getPluginsInfo());
			
			std::vector<std::optional<typename T::Result>> result;
			for (const int i : chosenReceivers)
				result.push_back(receiversCollection->at(i).second(event));

			return result;
		}
		else
			return {};
	}
	
} // namespace pp