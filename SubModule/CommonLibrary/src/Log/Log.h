#pragma once
#include <string>
#include <cstdint>
#include <memory>
#include <list>
#include <sstream>  // 用流的方式实现日志输出
#include <fstream>
#include <vector>
#include <map>

#include "CommonModule.h"
#include "Utils/Mutex.h"

namespace CommonModule {
	class Logger;

	class LogLevel {
	public:
#ifdef _WIN32
		enum Level { UNKNOW = 0, DEBUG = 1, INFO, WARN, ERROR_1, FATAL, MAX_LEVEL = 10000 };
#elif __linux__
		enum Level { UNKNOW = 0, DEBUG = 1, INFO, WARN, ERROR, FATAL, MAX_LEVEL = 10000 };
#endif
		static const char* ToString(LogLevel::Level level);
		static LogLevel::Level FromString( std::string& str);
	};

	class COMMONMODULE_API LogEvent {
	public:
		using ptr = std::shared_ptr<LogEvent>;

		LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int line, uint32_t elapse,
			uint32_t thread_id, uint64_t time, const std::string& threadName);
		~LogEvent() = default;
	public:
		const char* getFile()  { return _file; }
		int32_t getLine()  { return _line; }
		uint32_t getElapse()  { return _elapse; }
		uint32_t getThreadId()  { return _threadId; }
		uint64_t getTime()  { return _time; }
		std::string getThreadName()  { return _thread_name; }
		std::string getContent()  { return _ss.str(); }
		std::shared_ptr<Logger> getLogger()  { return _logger; }
		LogLevel::Level getLevel()  { return _level; }

		void format( char* fmt, ...);
		void format( char* fmt, va_list al);

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
	class COMMONMODULE_API LogEventWarp {
	public:
		LogEventWarp(LogEvent::ptr event);
		~LogEventWarp();

		std::stringstream& getSS();
		LogEvent::ptr getEvent() { return _event; }
	private:
		LogEvent::ptr _event;
	};

	// 日志格式
	class COMMONMODULE_API LogFormatter {
	public:
		typedef std::shared_ptr<LogFormatter> ptr;
		LogFormatter(const std::string& pattern);

		// %t   %threadID %m%n
		std::string format( std::shared_ptr<Logger>& logger, LogLevel::Level level, LogEvent::ptr event);
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
		bool isError()  { return _is_error; }
		 std::string getPattern()  { return _pattern; }
	private:
		std::string _pattern;
		std::vector<FormatItem::ptr> _items;
		bool _is_error = false;
	};

	// 日志输出位置
	class COMMONMODULE_API LogAppender {
		friend class Logger;
	public:
		typedef std::shared_ptr<LogAppender> ptr;
		typedef CASLock MutexType;
		LogAppender() = default;
		virtual ~LogAppender() = default;

		virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
		virtual std::string toXMLString() = 0;
		virtual std::string getAppenderName() = 0;

		void setFormatter(LogFormatter::ptr val);
		LogFormatter::ptr getFormatter()  { return _formatter; }
		LogLevel::Level getLevel()  { return _level; }
		void setLevel(LogLevel::Level val) { _level = val; }

	protected:
		LogLevel::Level				 _level = LogLevel::Level::DEBUG;
		bool						 _hasFormatter = false;
		mutable MutexType			 _mutex;
		LogFormatter::ptr			 _formatter;
	};

	// 日志定义类别  负责对外接口
	class COMMONMODULE_API Logger : public std::enable_shared_from_this<Logger> {
	public:
		typedef std::shared_ptr<Logger> ptr;
		typedef CASLock MutexType;
		Logger(std::string name = "root");
		// 日志输出接口
		void log(LogLevel::Level level,  LogEvent::ptr event);
		std::string toXMLString();

		void debug(LogEvent::ptr event);
		void info(LogEvent::ptr event);
		void warn(LogEvent::ptr event);
		void error(LogEvent::ptr event);
		void fatal(LogEvent::ptr event);

		void addAppender(const LogAppender::ptr& appender);
		void delAppender(const LogAppender::ptr& appender);
		void clearAppenders();
		[[nodiscard]] LogLevel::Level getLevel()  { return _level; }
		void setLevel(LogLevel::Level val) { _level = val; }
		void setLogger(Logger::ptr val) { _root = val; }

		 std::string& getName()  { return _name; }
		void setFormatter(LogFormatter::ptr val);
		void setFormatter( std::string& val);
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
// 	class StdoutLogAppender : public LogAppender {
// 	public:
// 		typedef std::shared_ptr<StdoutLogAppender> ptr;
// 		void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
// 		std::string toXMLString() override;
// 	};

	// 输出到文件的Appender  可能要在析构函数中关闭文件描述符，但是代码里好像没写
	class COMMONMODULE_API FileLogAppender : public LogAppender {
	public:
		typedef std::shared_ptr<FileLogAppender> ptr;
		FileLogAppender( std::string& filename,  std::string strAppenderName = "File");

		void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
		std::string toXMLString() override;
		std::string getAppenderName() override { return _strAppenderName; };
		void SetLogFilePath(const std::string& strFilePath);

		bool reopen();

	private:
		std::string				_filename;
		std::ofstream			_filestream;
		uint64_t				_last_time;
		std::string				_strAppenderName;
	};
}