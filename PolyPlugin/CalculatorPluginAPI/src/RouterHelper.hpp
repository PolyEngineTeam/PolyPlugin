#pragma once

#include <pp/IntentRouter.hpp>
#include <AddIntent.hpp>
#include <SubstractIntent.hpp>

//------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::optional<typename T::Result> processIntent(pp::IntentRouter& router, T intent)
{
	return router.processIntent<T>(std::move(intent));
}