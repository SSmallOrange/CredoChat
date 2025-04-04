#include "util.h"

#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <windows.h>
#include <filesystem>

namespace CommonModule {

	// 获取当前工作目录
	std::string CurrentWorkPath() {
		std::filesystem::path currentPath = std::filesystem::current_path();
		return currentPath.string();
	}

	// 获取当前可执行文件路径
	std::string CurrentExecPath() {
		wchar_t buffer[MAX_PATH];
		// 获取当前程序的路径，返回值是 LPWSTR
		DWORD result = GetModuleFileName(nullptr, buffer, MAX_PATH);

		// 检查 GetModuleFileName 是否成功
		if (result == 0) {
			// 获取失败，输出错误信息
			DWORD error_code = GetLastError();
			std::cerr << "GetModuleFileName failed with error code: " << error_code << std::endl;
			return "";
		}

		// 将 LPWSTR 转换为 std::string
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, nullptr, 0, nullptr, nullptr);
		if (size_needed > 0) {
			std::string result(size_needed, 0);
			int bytes_converted = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, &result[0], size_needed, nullptr, nullptr);

			// 检查是否转换成功
			if (bytes_converted == 0) {
				DWORD error_code = GetLastError();
				std::cerr << "WideCharToMultiByte failed with error code: " << error_code << std::endl;
				return "";
			}
			size_t pos = result.find_last_of("\\");
			if (pos != std::string::npos) {
				// 截断字符串，只保留路径部分
				result = result.substr(0, pos);
			}
			return result;
		}

		// 转换失败
		std::cerr << "WideCharToMultiByte failed: size_needed is 0" << std::endl;
		return "";
	}

	// 获取配置文件路径
	std::string CurrentConfigFilePath() {
		std::string currentExecPath = CurrentExecPath();
		
		if (currentExecPath != "") {
			std::filesystem::path curPath(currentExecPath);
			std::filesystem::path parentPath = curPath.parent_path();
			return parentPath.string() + "LocalCfg.xml";
		}
	}

	// 获取当前ThreadID
	uint32_t GetCurrentThreadId() {
		// 当前线程 ID
		std::thread::id tid = std::this_thread::get_id();
		size_t tid_hash = std::hash<std::thread::id>{}(tid);
		return static_cast<uint32_t>(tid_hash);
	}

	// 获取当前时间
	std::string GetCurrentTimestamp(std::string strFormat) {
		auto now = std::chrono::system_clock::now();
		auto in_time_t = std::chrono::system_clock::to_time_t(now);

		struct tm tm;
		localtime_s(&tm, &in_time_t);
		char buf[64];
		strftime(buf, sizeof(buf), strFormat.c_str(), &tm);
		return buf;
	}
}