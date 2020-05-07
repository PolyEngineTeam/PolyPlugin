#pragma once

#include <string>
#include <optional>

#include <pp/Info.hpp>

namespace pp
{
	class LogEvent
	{
	public:
		using Result = bool;
		static inline pp::EventInfo Info = { "LogEvent", 3 };

		enum class eLogLevel
		{
			ALL,

			DEBUG,
			TRACE,
			INFO,
			WARN,
			ERROR,
			FATAL,

			OFF
		};

		std::string message;
		eLogLevel level = eLogLevel::ALL;
		std::optional<std::string> channel;
	};
}