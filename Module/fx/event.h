// https://github.com/citizenfx/fivem/blob/master/code/client/shared/EventCore.h
#pragma once
#include "ref_container.h"

#include <functional>

namespace fx
{
template<typename... Args>
class fwEvent
{
public:
	using TFunc = std::function<bool(Args...)>;

private:
	struct callback
	{
		TFunc function;
		std::unique_ptr<callback> next = nullptr;
		int order = 0;
		size_t cookie = -1;

		callback(TFunc func)
			: function(func)
		{
		}
	};

	std::unique_ptr<callback> m_callbacks;
	std::atomic<size_t> m_connectCookie = 0;

public:
	fwEvent()
	{
		m_callbacks = nullptr;
	}

	~fwEvent()
	{
		Reset();
	}

	template<typename T>
	auto Connect(T func)
	{
		return Connect(func, 0);
	}

	template<typename T>
	auto Connect(T func, int order)
	{
		if constexpr (std::is_same_v<std::invoke_result_t<T, Args...>, bool>)
		{
			return ConnectInternal(func, order);
		}
		else
		{
			return ConnectInternal([func](Args&&... args)
			{
				std::invoke(func, args...);
				return true;
			},
			order);
		}
	}

	void Reset()
	{
		m_callbacks.reset();
	}

private:
	size_t ConnectInternal(TFunc func, int order)
	{
		if (!func)
			return -1;

		auto cookie = m_connectCookie++;
		auto cb = std::unique_ptr<callback>(new callback(func));
		cb->order = order;
		cb->cookie = cookie;

		if (!m_callbacks)
		{
			m_callbacks = std::move(cb);
		}
		else
		{
			auto cur = &m_callbacks;
			callback* last = nullptr;

			while (*cur && order >= (*cur)->order)
			{
				last = cur->get();
				cur = &(*cur)->next;
			}

			cb->next = std::move(*cur);
			(!last ? m_callbacks : last->next) = std::move(cb);
		}

		return cookie;
	}

public:
	void Disconnect(size_t cookie)
	{
		if (cookie == -1)
			return;

		callback* prev = nullptr;

		for (auto cb = m_callbacks.get(); cb; cb = cb->next.get())
		{
			if (cb->cookie == cookie)
			{
				if (prev)
					prev->next = std::move(cb->next);
				else
					m_callbacks = std::move(cb->next);

				break;
			}

			prev = cb;
		}
	}

	auto ConnectInternal(TFunc func)
	{
		return ConnectInternal(func, 0);
	}

public:
	operator bool() const
	{
		return m_callbacks.get() != nullptr;
	}

	bool operator()(Args... args) const
	{
		if (!m_callbacks)
			return true;

		for (auto cb = m_callbacks.get(); cb; cb = cb->next.get())
		{
			if (!std::invoke(cb->function, args...))
				return false;
		}

		return true;
	}
};
} // fx