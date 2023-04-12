#include "scripthost.h"

#include "../scripting/vfs_hook.h"

namespace fx
{
uint32_t TestScriptHost::OpenHostFile_hook(TestScriptHost* _this, char* file, StreamWrapper** stream)
{
	*(scripting::HookedStreamWrapper**)stream = new scripting::HookedStreamWrapper;
	return 0;
}
} // fx