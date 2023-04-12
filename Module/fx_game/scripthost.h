#pragma once
#include "../fx/interface.h"

namespace fx
{
typedef void* fxIStream;
typedef void* fxNativeContext;

class Resource;
class StreamWrapper;

class TestScriptHost : public IBase
{
public:
	virtual uint32_t InvokeNative(fxNativeContext& context) = 0;

	virtual uint32_t OpenSystemFile(char* fileName, fxIStream** stream) = 0;

	virtual uint32_t OpenHostFile(char* fileName, fxIStream** stream) = 0;

	virtual uint32_t CanonicalizeRef(int32_t refIdx, int32_t instanceId, char** outRefText) = 0;

	virtual uint32_t ScriptTrace(char* string) = 0;

	virtual uint32_t SubmitBoundaryStart(char* boundaryData, int boundarySize) = 0;

	virtual uint32_t SubmitBoundaryEnd(char* boundaryData, int boundarySize) = 0;

	virtual uint32_t GetLastErrorText(char** text) = 0;

public:
	static uint32_t OpenHostFile_hook(TestScriptHost* _this, char* file, StreamWrapper** stream);
};

class IScriptHostWithResourceData : public IBase
{
public:
	virtual uint32_t GetResourceName(char** outResourceName) = 0;

	virtual uint32_t GetNumResourceMetaData(char* metaDataName, int32_t* entryCount) = 0;

	virtual uint32_t GetResourceMetaData(char* metaDataName, int32_t entryIndex, char** outMetaData) = 0;
};

class IScriptHostWithManifest : public IBase
{
public:
	virtual uint32_t IsManifestVersionBetween(const guid_t& lowerBound, const guid_t& upperBound, bool* _retval) = 0;

	virtual uint32_t IsManifestVersionV2Between(char* lowerBound, char* upperBound, bool* _retval) = 0;
};

class IScriptTickRuntimeWithBookmarks;

class IScriptHostWithBookmarks : public IBase
{
public:
	virtual uint32_t ScheduleBookmark(IScriptTickRuntimeWithBookmarks* scRT, uint64_t bookmark, int64_t deadline) = 0;

	virtual uint32_t RemoveBookmarks(IScriptTickRuntimeWithBookmarks* scRT) = 0;
};

class IScriptHost : public TestScriptHost, public IScriptHostWithResourceData, public IScriptHostWithManifest, public IScriptHostWithBookmarks
{
private:
	char pad[0x8];

public:
	Resource* m_resource;
};
} // fx