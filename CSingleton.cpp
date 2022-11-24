#include "CSingleton.h"
#include <vector>

namespace
{
	std::mutex gMutex;
	std::vector<CSingletonFinalizer::Func> gFinalize;
}

void CSingletonFinalizer::AddFinalize(Func func)
{
	std::lock_guard<std::mutex> lock(gMutex);
	gFinalize.emplace_back(func);
}

void CSingletonFinalizer::Finalize()
{
	std::lock_guard<std::mutex> lock(gMutex);
	for (size_t i = 0; i < gFinalize.size(); i++)
	{
		(*gFinalize[i])();
	}
	gFinalize.clear();
}
