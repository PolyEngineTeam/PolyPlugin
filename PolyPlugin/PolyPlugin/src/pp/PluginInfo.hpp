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
		std::string name;
		Version version;
	};
}