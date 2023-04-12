// https://github.com/citizenfx/fivem/blob/master/code/components/citizen-scripting-core/include/fxScripting.idl
#pragma once
#include "../fx/interface.h"

#include <string>

namespace fx
{
class IScriptHostWithResourceData;
class IScriptHost;
class IScriptHostWithBookmarks;
class IScriptHostWithResourceData;
class IScriptHostWithManifest;

class IScriptTickRuntime : public IBase
{
public:
	virtual uint32_t Tick() = 0;
};

class IScriptTickRuntimeWithBookmarks : public IBase
{
public:
	virtual uint32_t TickBookmarks(uint64_t* bookmarks, int numBookmarks) = 0;
};

class IScriptFileHandlingRuntime : public IBase
{
public:
	virtual uint32_t HandlesFile(char* fileName, IScriptHostWithResourceData* metadata) = 0;

	virtual uint32_t LoadFile(char* scriptName) = 0;
};

class IScriptEventRuntime : public IBase
{
public:
	virtual uint32_t TriggerEvent(char* eventName, char* eventPayload, uint32_t payloadSize, char* eventSource) = 0;
};

class IScriptRefRuntime : public IBase
{
public:
	virtual uint32_t CallRef(int32_t refIdx, char* argsSerialized, uint32_t argsLength, char** retvalSerialized, uint32_t* retvalLength) = 0;

	virtual uint32_t DuplicateRef(int32_t refIdx, int32_t* outRef) = 0;

	virtual uint32_t RemoveRef(int32_t refIdx) = 0;
};

class IScriptMemInfoRuntime : public IBase
{
public:
	virtual uint32_t RequestMemoryUsage() = 0;

	virtual uint32_t GetMemoryUsage(int64_t* memoryUsage) = 0;
};

class IScriptStackWalkVisitor : public IBase
{
public:
	virtual uint32_t SubmitStackFrame(char* frameBlob, uint32_t frameBlobSize) = 0;
};

class IScriptStackWalkingRuntime : public IBase
{
public:
	virtual uint32_t WalkStack(char* boundaryStart, uint32_t boundaryStartLength, char* boundaryEnd, uint32_t boundaryEndLength, IScriptStackWalkVisitor* visitor) = 0;
};

class IDebugEventListener : public IBase
{
public:
	virtual uint32_t OnBreakpointsDefined(int scriptId, char* breakpointJson) = 0;
};

class IScriptDebugRuntime : public IBase
{
public:
	virtual uint32_t SetDebugEventListener(IDebugEventListener* listener) = 0;

	virtual uint32_t SetScriptIdentifier(char* fileName, int32_t scriptId) = 0;
};

class IScriptProfiler : public IBase
{
public:
	virtual uint32_t SetupFxProfiler(void* obj, int32_t resourceId) = 0;

	virtual uint32_t ShutdownFxProfiler() = 0;
};

class IScriptRuntime : public IBase
{
public:
	virtual uint32_t Create(IScriptHost* scriptHost) = 0;

	virtual uint32_t Destroy() = 0;

	virtual void* GetParentObject() = 0;

	virtual void SetParentObject(void* object) = 0;

	virtual int GetInstanceId() = 0;

public:
	template<typename TRuntime>
	TRuntime* get_impl()
	{
		return (TRuntime*)&((uint64_t*)this)[-1];
	}
};

class LuaScriptRuntime : public IScriptRuntime, public IScriptFileHandlingRuntime, public IScriptTickRuntimeWithBookmarks, public IScriptEventRuntime, public IScriptRefRuntime, public IScriptMemInfoRuntime, public IScriptStackWalkingRuntime, public IScriptDebugRuntime, public IScriptProfiler
{
private:
	char pad[0x20];

public:
	IScriptHost* m_scriptHost;
	IScriptHostWithBookmarks* m_bookmarkHost;
	IScriptHostWithResourceData* m_resourceHost;
	IScriptHostWithManifest* m_manifestHost;

public:
	void execute_script(const std::string& script, const std::string& scriptName);
};
} // fx