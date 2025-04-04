#pragma once
#include "CommonModule.h"

#include <string>

namespace CommonModule {
	// 获取当前工作目录
	COMMONMODULE_API std::string CurrentWorkPath();

	// 获取当前可执行文件路径
	COMMONMODULE_API std::string CurrentExecPath();

	// 获取配置文件路径
	COMMONMODULE_API std::string CurrentConfigFilePath();

	// 获取当前线程ID
	COMMONMODULE_API uint32_t GetCurrentThreadId();

	// 获取当前时间 
	COMMONMODULE_API std::string GetCurrentTimestamp(std::string strFormat = "%Y-%m-%d_%H-%M-%S");
}