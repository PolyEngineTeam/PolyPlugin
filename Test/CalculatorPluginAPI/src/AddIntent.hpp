#pragma once

#include <pp/Info.hpp>

//------------------------------------------------------------------------------------------------------------------------------------------
class AddIntent
{
public:
	using Result = int;
	static inline pp::IntentInfo Info = { "AddIntent", 3 };

	int a = 0;
	int b = 0;
};