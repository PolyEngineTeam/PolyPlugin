#pragma once

#include <memory>
#include <optional>
#include <map>
#include <cassert>

#include <pp/ReceiversCollection.hpp>

namespace pp
{
	class ReceiverSelector;

	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	// This class' purpose is to allow registering/unregistering intent
	// handlers and provide interface for intent dispatching.
	class IntentRouter
	{
	public:
		// Create intent router with default receiver selector which 
		// will always select the first registered intent handler.
		IntentRouter() : m_selector(std::make_shared<ReceiverSelector>()) {}

		// Allows user to provide custom receiver selector.
		// @parem selector - receiver selector provided by user
		IntentRouter(std::shared_ptr<ReceiverSelector> selector) : m_selector(std::move(selector)) {}

		// Registers intent receiver/handler in this router. If someone
		// will dispatch intent with type matching given handler with 
		// 'processIntent' method the registered handler will be sent 
		// to the selector along with the other handlers matching 
		// intent type and selector will choose which handler should 
		// process the intent.
		// @tparam T - type of an intent which can be processed by the given
		//		'receiver
		// @param info - plugin info is not necessary for this class to
		//		work but it might be useful to be able to present to 
		//		the user names and versions of plugins that are able to 
		//		process the intent if there are multiple intent 
		//		handlers that match the intent type.
		// @param receiver - intent receiver to register
		template <typename T>
		void registerReceiver(PluginInfo info, std::function<typename T::Result(T)> receiver);

		// Unregisters all receivers registered with given plugin info.
		// @param info - info from plugin which intent receivers should
		//		be removed
		void unregisterReceivers(PluginInfo info) { assert(false); }

		// This method is used for intents dispatching. When it's 
		// called it asks receiver selector which intent receiver 
		// should be used to process given intent.
		// @tparem T - type of the intent that needs to be processed
		// @returns empty optional if receiver for given intent wasn't 
		//		found or IntentType::Result returned from intent 
		//		receiver.
		// @param intent - intent that needs to be processed
		template <typename T>
		std::optional<typename T::Result> processIntent(T intent);

	private:
		std::shared_ptr<ReceiverSelector> m_selector;

		// Receivers are mapped by intent info so older intents (with 
		// older version) also can be processed if receiver with 
		// matching intent type is registered in this intent router.
		std::map<IntentInfo, std::unique_ptr<ReceiversCollectionBase>> m_receivers;
	}; // class IntentRouter

	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	// Helper class for intent router. This class is meant to be 
	// overridden by the user so it will be possible to implement 
	// choosing the default intent receiver (like in android when some
	// action is taken for the first time and there are more than one 
	// plugins that can handle this intent, for example opening photo 
	// gallery to select a picture).
	class ReceiverSelector
	{
	public:
		virtual ~ReceiverSelector() = default;

		// This method is called every time intent router has to 
		// process the intent.Default implementation chooses the first
		// receiver (the first that was registered for this type of 
		// intent).
		// @returns index of the receiver present in the given 
		//		'receivers' collection that should process the intent
		//		with given 'intentInfo'
		// @param intent - intent info of an intent that needs to be 
		//		processed
		// @param receivers - collection of available receivers
		virtual int selectReceiver(IntentInfo intent, std::vector<PluginInfo> receivers) { return 0; }
	}; // class ReceiverSelector

	//-------------------------------------------------------------------------------------------------------
	template<typename T>
	inline void IntentRouter::registerReceiver(PluginInfo info, std::function<typename T::Result(T)> receiver)
	{
		const auto it = m_receivers.find(T::Info);
		if (it != m_receivers.end())
			static_cast<ReceiversCollection<T>*>(it->second.get())->push_back({ std::move(info), std::move(receiver) });
		else
		{
			auto newCollection = std::make_unique<ReceiversCollection<T>>();
			newCollection->push_back({ std::move(info), std::move(receiver) });
			m_receivers.insert({ T::Info, std::move(newCollection) });
		}
	}

	//-------------------------------------------------------------------------------------------------------
	template<typename T>
	inline std::optional<typename T::Result> IntentRouter::processIntent(T intent)
	{
		const auto it = m_receivers.find(T::Info);
		if (it != m_receivers.end())
		{
			const auto receiversCollection = static_cast<ReceiversCollection<T>*>(it->second.get());
			const int chosenReceiver = m_selector->selectReceiver(T::Info, receiversCollection->getPluginsInfo());
			return receiversCollection->at(chosenReceiver).second(std::move(intent));
		}
		else
			return {};
	}

} // namespace pp
