#pragma once

#include <vector>
#include <functional>
#include <utility>

#include <pp/IntentInfo.hpp>
#include <pp/PluginInfo.hpp>

namespace pp
{
	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	// This is the base class for containers of intent receivers.
	class ReceiversCollectionBase
	{
	public:
		virtual ~ReceiversCollectionBase() = default;

		// @returns intent info of intent that can be handled by 
		//		receivers stored in this collection
		virtual const IntentInfo& getIntentInfo() const = 0;

		// @returns plugin info of all registered intent receivers 
		//		(indices match) so we can present available plugin 
		//		receivers to the user before casting the collection and
		//		accessing the receivers directly
		virtual std::vector<PluginInfo> getPluginsInfo() const = 0;
	};

	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	// This is the concrete container for receivers of the specific 
	// intent type
	// @tparam T - type of an intent that can be processed by all 
	//		receivers present in this collection
	template <typename T>
	class ReceiversCollection :
		public ReceiversCollectionBase,
		public std::vector<std::pair<PluginInfo, std::function<typename T::Result(T)>>>
	{
	public:
		const IntentInfo& getIntentInfo() const final { return T::Info; }
		std::vector<PluginInfo> getPluginsInfo() const final
		{
			std::vector<PluginInfo> result;
			for (auto[key, value] : *this)
				result.push_back(key);
			return result;
		}
	};
} // namespace pp