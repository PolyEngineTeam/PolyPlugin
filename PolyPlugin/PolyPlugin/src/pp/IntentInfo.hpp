#pragma once

#include <string>

namespace pp
{
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
} // namespace pp