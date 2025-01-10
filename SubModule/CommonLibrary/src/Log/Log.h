#pragma once
#include <string>
#include <cstdint>
#include <memory>
#include <list>
#include <sstream>  // 用流的方式实现日志输出
#include <fstream>
#include <vector>
#include <map>

#include "Utils/Mutex.h"

namespace CommonModule {
	class Logger;

	class LogLevel {
	public:
		enum Level { UNKNOW = 0, DEBUG = 1, INFO, WARN, ERROR, FATAL, MAX_LEVEL = 10000 };
		static const char* ToString(LogLevel::Level level);
		static LogLevel::Level FromString(const std::string& str);
	};

	class LogEvent {
	public:
		using ptr = std::shared_ptr<LogEvent>;

		LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int line, uint32_t elapse,
			uint32_t thread_id, uint64_t time, const std::string& threadName);
		~LogEvent() = default;
	public:
		const char* getFile() const { return _file; }
		int32_t getLine() const { return _line; }
		uint32_t getElapse() const { return _elapse; }
		uint32_t getThreadId() const { return _threadId; }
		uint64_t getTime() const { return _time; }
		std::string getThreadName() const { return _thread_name; }
		std::string getContent() const { return _ss.str(); }
		std::shared_ptr<Logger> getLogger() const { return _logger; }
		LogLevel::Level getLevel() const { return _level; }

		void format(const char* fmt, ...);
		void format(const char* fmt, va_list al);

		std::stringstream& getSS() { return _ss; }
	private:
		const char* _file = nullptr;  // 文件
		int32_t _line = 0;            // 行号
		uint32_t _elapse = 0;         // 程序启动开始到现在的毫秒数
		uint32_t _threadId = 0;       // 线程ID
		uint64_t _time = 0;           // 时间戳
		std::string _thread_name;
		std::stringstream _ss;        // 内容

		std::shared_ptr<Logger> _logger;  // 要写到的目标对象
		LogLevel::Level _level;
	};

	// 日志输出用
	class LogEventWarp {
	public:
		LogEventWarp(LogEvent::ptr event);
		~LogEventWarp();

		std::stringstream& getSS();
		LogEvent::ptr getEvent() { return _event; }
	private:
		LogEvent::ptr _event;
	};

	// 日志格式
	class LogFormatter {
	public:
		typedef std::shared_ptr<LogFormatter> ptr;
		LogFormatter(const std::string& pattern);

		// %t   %threadID %m%n
		std::string format(const std::shared_ptr<Logger>& logger, LogLevel::Level level, LogEvent::ptr event);
		std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
	public:
		class FormatItem {
		public:
			FormatItem(const std::string& fmt = "") {}
			typedef std::shared_ptr<FormatItem> ptr;
			virtual ~FormatItem() = default;
			virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
		};
		void Init();
		bool isError() const { return _is_error; }
		const std::string getPattern() const { return _pattern; }
	private:
		std::string _pattern;
		std::vector<FormatItem::ptr> _items;
		bool _is_error = false;
	};

	// 日志输出位置
	class LogAppender {
		friend class Logger;
	public:
		typedef std::shared_ptr<LogAppender> ptr;
		typedef CASLock MutexType;
		LogAppender() = default;
		virtual ~LogAppender() = default;
		virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
		virtual void toXMLString() = 0;
		virtual std::string toYamlString() = 0;
		void setFormatter(LogFormatter::ptr val);
		LogFormatter::ptr getFormatter() const;
		LogLevel::Level getLevel() const { return _level; }
		void setLevel(LogLevel::Level val) { _level = val; }
	protected:
		LogLevel::Level _level = LogLevel::Level::DEBUG;
		bool _hasFormatter = false;
		mutable MutexType _mutex;
		LogFormatter::ptr _formatter;
	};

	// 日志定义类别  负责对外接口
	class Logger : public std::enable_shared_from_this<Logger> {
	public:
		typedef std::shared_ptr<Logger> ptr;
		typedef CASLock MutexType;
		Logger(std::string name = "root");
		// 日志输出接口
		void log(LogLevel::Level level, const LogEvent::ptr event);
		std::string toXMLString();

		void debug(LogEvent::ptr event);
		void info(LogEvent::ptr event);
		void warn(LogEvent::ptr event);
		void error(LogEvent::ptr event);
		void fatal(LogEvent::ptr event);

		void addAppender(const LogAppender::ptr& appender);
		void delAppender(const LogAppender::ptr& appender);
		void clearAppenders();
		[[nodiscard]] LogLevel::Level getLevel() const { return _level; }
		void setLevel(LogLevel::Level val) { _level = val; }

		const std::string& getName() const { return _name; }
		void setFormatter(LogFormatter::ptr val);
		void setFormatter(const std::string& val);
		LogFormatter::ptr getFormatter();

	private:
		std::string _name;       // 日志名称
		LogLevel::Level _level;  
		MutexType _mutex;
		std::list<LogAppender::ptr> _appenders;         //Appender集合
		LogFormatter::ptr _formatter;
		Logger::ptr _root;
	};

	// 输出到控制台的Appender
	class StdoutLogAppender : public LogAppender {
	public:
		typedef std::shared_ptr<StdoutLogAppender> ptr;
		void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
		void toXMLString() override;
	};

	// 输出到文件的Appender  可能要在析构函数中关闭文件描述符，但是代码里好像没写
	class FileLogAppender : public LogAppender {
	public:
		typedef std::shared_ptr<FileLogAppender> ptr;
		FileLogAppender(const std::string& filename);
		void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
		void toXMLString() override;
		// 重新打开文件， 打开成功返回true
		bool reopen();
	private:
		std::string _filename;
		std::ofstream _filestream;
		uint64_t _last_time;
	};
}