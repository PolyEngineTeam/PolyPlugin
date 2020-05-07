#pragma once

#include <string>

namespace pp
{
	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	class Version
	{						// https://semver.org/
	public:
		int major = 0;      // incompatible API changes
		int minor = 0;      // functionality added in a backwards compatible manner
		int patch = 0;      // backwards compatible bug fixes
	};

	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	// The main purpose of this class is to enable PolyPlugin to 
	// identify plugins somehow and display plugin name and version to
	// the user so he/she will be able to select which intent receiver 
	// should be used to process the intent.
	class PluginInfo
	{
	public:
		const std::string name;
		const Version version;
	};

	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	// This is class that contains information about the intent info.
	class IntentInfo
	{
	public:
		// Intents have its name to distinguish intents. There can't be
		// two intents with the same name otherwise the plugin behavior 
		// is undefined.
		std::string name;

		// There are no backwards compatible fixes in intents API so 
		// there is only major version of the intent. Intents are the
		// public API of every plugin in PolyPlugin if anything changes
		// in an intent its version number should be increased to 
		// prevent ABI incompatibility issues.
		int version = 0;
	};

	//-------------------------------------------------------------------------------------------------------
	bool operator== (const IntentInfo& left, const IntentInfo& right)
	{
		return left.name == right.name && left.version == right.version;
	}

	//-------------------------------------------------------------------------------------------------------
	bool operator!= (const IntentInfo& left, const IntentInfo& right)
	{
		return !(left == right);
	}

	//-------------------------------------------------------------------------------------------------------
	bool operator< (const IntentInfo& left, const IntentInfo& right)
	{
		return left.version < right.version || left.name < right.name;
	}

	//-------------------------------------------------------------------------------------------------------
	bool operator> (const IntentInfo& left, const IntentInfo& right)
	{
		return left.version > right.version || left.name > right.name;
	}

	//-------------------------------------------------------------------------------------------------------
	bool operator<= (const IntentInfo& left, const IntentInfo& right)
	{
		return left.version <= right.version || left.name <= right.name;
	}

	//-------------------------------------------------------------------------------------------------------
	bool operator>= (const IntentInfo& left, const IntentInfo& right)
	{
		return left.version >= right.version || left.name >= right.name;
	}

	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------
	// This is class that contains information about the event info.
	class EventInfo
	{
	public:
		// Events have its name to distinguish events. There can't be
		// two events with the same name otherwise the plugin behavior 
		// is undefined.
		std::string name;

		// There are no backwards compatible fixes in events API so 
		// there is only major version of the event. Events are the
		// public API of every plugin in PolyPlugin if anything changes
		// in an event its version number should be increased to 
		// prevent ABI incompatibility issues.
		int version = 0;
	};

	//-------------------------------------------------------------------------------------------------------
	bool operator== (const EventInfo& left, const EventInfo& right)
	{
		return left.name == right.name && left.version == right.version;
	}

	//-------------------------------------------------------------------------------------------------------
	bool operator!= (const EventInfo& left, const EventInfo& right)
	{
		return !(left == right);
	}

	//-------------------------------------------------------------------------------------------------------
	bool operator< (const EventInfo& left, const EventInfo& right)
	{
		return left.version < right.version || left.name < right.name;
	}

	//-------------------------------------------------------------------------------------------------------
	bool operator> (const EventInfo& left, const EventInfo& right)
	{
		return left.version > right.version || left.name > right.name;
	}

	//-------------------------------------------------------------------------------------------------------
	bool operator<= (const EventInfo& left, const EventInfo& right)
	{
		return left.version <= right.version || left.name <= right.name;
	}

	//-------------------------------------------------------------------------------------------------------
	bool operator>= (const EventInfo& left, const EventInfo& right)
	{
		return left.version >= right.version || left.name >= right.name;
	}
} // namespace pp