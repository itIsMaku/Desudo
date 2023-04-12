// https://github.com/citizenfx/fivem/blob/master/code/client/citicore/om/OMPtr.h
#pragma once

namespace fx
{
template<typename T>
class OMPtr
{
private:
	T* m_ref;

public:
	OMPtr()
		: m_ref(nullptr)
	{
	}

	~OMPtr()
	{
		if (m_ref)
		{
			if (m_ref->Release())
				m_ref = nullptr;
		}
	}

	OMPtr(T* ref)
	{
		m_ref = ref;

		if (m_ref)
			m_ref->AddRef();
	}

	OMPtr(const OMPtr& rc)
	{
		m_ref = rc.m_ref;

		if (m_ref)
			m_ref->AddRef();
	}

	uint32_t GetRefCount() const
	{
		return m_ref->GetRefCount();
	}

	T* GetRef() const
	{
		return m_ref;
	}

	T* operator->() const
	{
		return m_ref;
	}

	OMPtr& operator=(const OMPtr& other)
	{
		if (m_ref)
			m_ref->Release();

		m_ref = other.GetRef();

		if (m_ref)
			m_ref->AddRef();

		return *this;
	}

	inline T** GetAddressOf()
	{
		return &m_ref;
	}

	inline T** ReleaseAndGetAddressOf()
	{
		if (m_ref)
		{
			if (m_ref->Release())
				m_ref = nullptr;
		}

		return GetAddressOf();
	}
};
} // fx