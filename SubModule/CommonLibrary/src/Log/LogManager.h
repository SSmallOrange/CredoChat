#pragma once
#include "Log.h"
#include "utils/Mutex.h"
#include "utils/singleton.h"
#include "CommonModule.h"

namespace CommonModule {
	class COMMONMODULE_API LoggerManager : Singleton<LoggerManager> {
	public:
		typedef Mutex MutexType;

	public:
		~LoggerManager() = default;

	private:
		friend class Singleton<LoggerManager>;  // 方便单例调用构造函数进行构造
		LoggerManager();

		Logger::ptr getLogger(const std::string& name);
		void init();
		std::string toXMLString();
		Logger::ptr getRoot() const { return _root; }
	private:
		MutexType m_mutex;
		/// 日志器容器
		std::map<std::string, Logger::ptr> _loggers;
		/// 主日志器
		Logger::ptr _root;
	};

}