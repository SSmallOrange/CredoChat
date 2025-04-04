#pragma once
#include "Log.h"
#include "Utils/util.h"
#include "utils/Mutex.h"
#include "utils/singleton.h"
#include "CommonModule.h"


#define LOG_LEVEL(logger, level) \
  if (logger->getLevel() <= level)    \
    CommonModule::LogEventWarp(CommonModule::LogEvent::ptr(new CommonModule::LogEvent(logger, level, __FILE__, __LINE__, 0, CommonModule::GetCurrentThreadId(), \
                 time(nullptr), std::to_string(CommonModule::GetCurrentThreadId())))).getSS()

#define LOG_DEBUG(logger) LOG_LEVEL(logger, CommonModule::LogLevel::Level::DEBUG)
#define LOG_INFO(logger) LOG_LEVEL(logger, CommonModule::LogLevel::Level::INFO)
#define LOG_WARN(logger) LOG_LEVEL(logger, CommonModule::LogLevel::Level::WARN)
#define LOG_ERROR(logger) LOG_LEVEL(logger, CommonModule::LogLevel::Level::ERROR)
#define LOG_FATAL(logger) LOG_LEVEL(logger, CommonModule::LogLevel::Level::FATAL)

#define LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        CommonModule::LogEventWarp(CommonModule::LogEvent::ptr(new CommonModule::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, CommonModule::GetCurrentThreadId(),\
                 time(nullptr), std::to_string(CommonModule::GetCurrentThreadId())))).getEvent()->format(fmt, __VA_ARGS__)

#define LOG_FMT_DEBUG(logger, fmt, ...) LOG_FMT_LEVEL(logger, CommonModule::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define LOG_FMT_INFO(logger, fmt, ...)  LOG_FMT_LEVEL(logger, CommonModule::LogLevel::INFO, fmt, __VA_ARGS__)
#define LOG_FMT_WARN(logger, fmt, ...)  LOG_FMT_LEVEL(logger, CommonModule::LogLevel::WARN, fmt, __VA_ARGS__)
#define LOG_FMT_ERROR(logger, fmt, ...) LOG_FMT_LEVEL(logger, CommonModule::LogLevel::ERROR, fmt, __VA_ARGS__)
#define LOG_FMT_FATAL(logger, fmt, ...) LOG_FMT_LEVEL(logger, CommonModule::LogLevel::FATAL, fmt, __VA_ARGS__)

#define LOG_ROOT() CommonModule::LoggerManager::GetInstance()->getRoot()
#define LOG_NAME(name) CommonModule::LoggerManager::GetInstance()->getLogger(name)


namespace CommonModule {
	class COMMONMODULE_API LoggerManager : public Singleton<LoggerManager> {
	public:
		typedef Mutex MutexType;

	public:
		~LoggerManager() = default;

	private:
		friend class Singleton<LoggerManager>;  // 方便单例调用构造函数进行构造
		LoggerManager();

	public:
		Logger::ptr getLogger(const std::string& name);
		void init();
		std::string toXMLString();
		Logger::ptr getRoot() const { return _root; }
	private:
		MutexType _mutex;
		/// 日志器容器
		std::map<std::string, Logger::ptr> _loggers;
		/// 主日志器
		Logger::ptr _root;
	};

}