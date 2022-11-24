#pragma once

#include <mutex>
#include <assert.h>

class CSingletonFinalizer
{
public:
	using Func = void(*)();
	static void AddFinalize(Func func);
	static void Finalize();
};

template<class T>
class CSingleton
{
public:
	static T* GetInstance()
	{
		std::call_once(initFlg, Create);
		assert(instance);
		return instance;
	}

private:
	static void Create() {
		instance = new T;
		CSingletonFinalizer::AddFinalize(&CSingleton<T>::Destroy);
	}

	static void Destroy() {
		delete instance;
		instance = nullptr;
	}

	static std::once_flag initFlg;
	static T* instance;
};

template<class T> std::once_flag CSingleton<T>::initFlg;
template<class T> T* CSingleton<T>::instance = nullptr;


