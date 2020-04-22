#pragma once

#include <pp/IntentInfo.hpp>

//------------------------------------------------------------------------------------------------------------------------------------------
class SubstractIntent
{
public:
	using Result = int;
	static inline pp::IntentInfo Info = { "SubstractIntent", 0 };

	int a = 0;
	int b = 0;
};