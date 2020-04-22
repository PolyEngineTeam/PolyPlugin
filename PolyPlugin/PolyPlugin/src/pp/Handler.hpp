#pragma once

#include <pp/IntentInfo.hpp>

namespace pp
{
	class IHandler
	{
	public:
		virtual IntentInfo getHandledIntentInfo() const = 0;
		virtual void* handleIntent(void* intent) = 0;
	};

	template <typename I>
	class HandlerImpl : public IHandler
	{
	public:
		IntentInfo getHandledIntentInfo() const final { return I::Info; }

		void* handleIntent(void* intent) final
		{
			return new typename I::Result(handleIntent(std::move(*static_cast<I*>(intent))));
		}

		virtual typename I::Result handleIntent(I intent) { return typename I::Result{}; };
	};

	//template <typename I>
	//class Handler
	//{
	//public:
	//	Handler(std::function<typename I::Result(I)> handler) : m_handler(std::move(handler)) {}
	//	Handler(std::shared_ptr<IHandler> handler) : m_handler(std::move(handler)) { assert(I::Info == handler->getHandledIntentInfo()); }
	//
	//	typename I::Result operator() (I intent)
	//	{
	//		if (std::holds_alternative<std::function<typename I::Result(I)>>(m_handler))
	//			return std::get<std::function<typename I::Result(I)>>(m_handler)(std::move(intent));
	//		else
	//		{
	//			void* result = std::get<std::shared_ptr<IHandler>>(m_handler)->handleIntent(&intent);
	//			auto* castedResult = static_cast<typename I::Result*>(result);
	//			typename I::Result ret = std::move(*castedResult);
	//			delete castedResult;
	//			return ret;
	//		}
	//	}
	//
	//private:
	//	std::variant<std::function<typename I::Result(I)>, std::shared_ptr<IHandler>> m_handler;
	//};
} // namespace pp