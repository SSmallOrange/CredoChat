#include "Log/Log.h"
#include "LocalConfig/LocalCfg.h"
#include "LocalConfig/XMLManager.h"

#include <cstdarg>
#include <ctime>
#include <functional>
#include <iostream>
#include <algorithm>

namespace XML = tinyxml2;

namespace CommonModule {
	const char* LogLevel::ToString(LogLevel::Level level) {
		switch (level)
		{
#define XX(name) \
      case LogLevel::name: \
        return #name;      \
        break;

			XX(DEBUG);
			XX(INFO);
			XX(WARN);
			XX(ERROR);
			XX(FATAL);
#undef XX
        default:
            return "UNKNOW";
		}
	}

	LogLevel::Level LogLevel::FromString(const std::string& str) {
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
#define XX(level, v) \
	if (str == #v) { \
		return LogLevel::level;  \
	}
		XX(DEBUG, debug);
		XX(INFO, info);
		XX(WARN, watn);
		XX(ERROR, error);
		XX(FATAL, fatal);
#undef XX
	}

	// ---------------- FormatItem -----------------------

	class MessageFormatItem : public LogFormatter::FormatItem {
	public:
		explicit MessageFormatItem(const std::string& str = "") {}
		void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getContent();
		}
	};

	class LevelFormatItem : public LogFormatter::FormatItem {
	public:
		explicit LevelFormatItem(const std::string& str = "") {}
		void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			// std::cout << "levelformatItem" << std::endl;
			os << LogLevel::ToString(level);
		}
	};

	class ElapseFormatItem : public LogFormatter::FormatItem {
	public:
		explicit ElapseFormatItem(const std::string& str = "") {}
		void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getElapse();
		}
	};

	class NameFormatItem : public LogFormatter::FormatItem {
	public:
		explicit NameFormatItem(const std::string& str = "") {}
		void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getLogger()->getName();
		}
	};

	class ThreadIdFormatItem : public LogFormatter::FormatItem {
	public:
		explicit ThreadIdFormatItem(const std::string& str = "") {}
		void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getThreadId();
		}
	};

	class ThreadNameFormatItem : public LogFormatter::FormatItem {
	public:
		explicit ThreadNameFormatItem(const std::string& str = "") {}
		void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getThreadName();
		}
	};

	class DateTimeFormatItem : public LogFormatter::FormatItem {
	public:
		explicit DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S") : _format(format) {
			if (_format.empty()) {
				_format = "%Y-%m-%d %H:%M:%S";
			}
		}

		void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			struct tm tm;
			time_t time = event->getTime();

			localtime_s(&tm, &time);
			char buf[64];
			strftime(buf, sizeof(buf), _format.c_str(), &tm);
			os << buf;
		}
	private:
		std::string _format;
	};

	class FileNameFormatItem : public LogFormatter::FormatItem {
	public:
		explicit FileNameFormatItem(const std::string& str = "") {}
		void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getFile();
		}
	};

	class LineFormatItem : public LogFormatter::FormatItem {
	public:
		explicit LineFormatItem(const std::string& str = "") {}
		void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getLine();
		}
	};

	class NewLineFormatItem : public LogFormatter::FormatItem {
	public:
		explicit NewLineFormatItem(const std::string& str = "") {}
		void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << std::endl;
		}
	};

	class StringFormatItem : public LogFormatter::FormatItem {
	public:
		explicit StringFormatItem(const std::string& str) : _string(str) {}
		void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << _string;
		}
	private:
		std::string _string;
	};

	class TableFormatItem : public LogFormatter::FormatItem {
	public:
		explicit TableFormatItem(const std::string& str = "") {}
		void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << "  ";
		}
	private:
		std::string _string;
	};

	// ---------------- Logger -----------------------
	Logger::Logger(std::string name) : _name(std::move(name)), _level(LogLevel::Level::DEBUG) {
		_formatter.reset(new LogFormatter("%d%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
	}

	void Logger::log(LogLevel::Level level, const LogEvent::ptr event) {
		if (level >= _level) {
			auto self = shared_from_this();
			MutexType::Lock lock(_mutex);
			if (!_appenders.empty()) {
				for (auto& i : _appenders) {
					i->log(self, level, event);
				}
			}
			else if (_root) {
				_root->log(level, event);
			}
		}
	}

	void Logger::debug(LogEvent::ptr event) {
		log(LogLevel::DEBUG, std::move(event));
	}

	void Logger::info(LogEvent::ptr event) {
		log(LogLevel::INFO, std::move(event));
	}

	void Logger::warn(LogEvent::ptr event) {
		log(LogLevel::WARN, std::move(event));
	}

	void Logger::error(LogEvent::ptr event) {
		log(LogLevel::ERROR, std::move(event));
	}

	void Logger::fatal(LogEvent::ptr event) {
		log(LogLevel::FATAL, std::move(event));
	}

	void Logger::addAppender(const LogAppender::ptr& appender) {
		MutexType::Lock lock(_mutex);
		if (!appender->getFormatter()) {
			MutexType::Lock ll(appender->_mutex);
			appender->_formatter = _formatter;
		}
		// std::cout << "addAppender" << std::endl;
		_appenders.push_back(appender);
	}

	void Logger::delAppender(const LogAppender::ptr& appender) {
		MutexType::Lock lock(_mutex);
		for (auto it = _appenders.begin(); it != _appenders.end(); it++) {
			if (appender == *it) {
				_appenders.erase(it);
				break;
			}
		}
	}

	void Logger::clearAppenders() {
		MutexType::Lock lock(_mutex);
		_appenders.clear();
	}

	void Logger::setFormatter(LogFormatter::ptr val) {
		MutexType::Lock lock(_mutex);
		_formatter = val;
		for (auto& i : _appenders) {
			i->setFormatter(val);
		}
	}

	void Logger::setFormatter(const std::string& val) {
		LogFormatter::ptr new_val(new LogFormatter(val));
		if (new_val->isError()) {
			std::cout << "Logger setFormatter name = " << _name
				<< " value = " << val << " invalid formatter" << std::endl;
			return;
		}
		setFormatter(new_val);
	}

	LogFormatter::ptr Logger::getFormatter() {
		MutexType::Lock lock(_mutex);
		return _formatter;
	}

	std::string Logger::toXMLString() {
		MutexType::Lock lock(_mutex);

		XMLCreateManager::ptr xmlCreateManager = std::make_shared<XMLCreateManager>();
		xmlCreateManager->InitDeclaration();

		xmlCreateManager->AddElementEnd(nullptr, "name", _name);

		if (_level != LogLevel::UNKNOW) {
			xmlCreateManager->AddElementEnd(nullptr, "level", LogLevel::ToString(_level));
		}
		if (_formatter) {
			xmlCreateManager->AddElementEnd(nullptr, "formatter", _formatter->getPattern());
		}
		XML::XMLElement* appendersXMLElement = xmlCreateManager->GetElement("appenders");
		for (auto& i : _appenders) {
			xmlCreateManager->AddElementEnd(appendersXMLElement, i->getAppenderName(), i->toXMLString());
		}
		std::stringstream ss;
		ss << xmlCreateManager->ConvertXMLToString();
		return ss.str();
	}

	// ---------------- LogAppender -----------------------
	void LogAppender::setFormatter(LogFormatter::ptr val) {
		MutexType::Lock lock(_mutex);
		_formatter = val;
	}

	// ---------------- FileLogAppender -----------------------
	FileLogAppender::FileLogAppender(const std::string& filename, const std::string strAppenderName) 
		: _filename(filename), _strAppenderName(strAppenderName) {
		reopen(); 
	}

	void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
		if (level >= _level) {
			uint64_t now = time(nullptr);
			if (now != _last_time) {
				reopen();
				_last_time = now;
			}
			MutexType::Lock lock(_mutex);
			if (!_formatter->format(_filestream, logger, level, event)) {
				std::cout << "error" << std::endl;
			}
		}
	}

	bool FileLogAppender::reopen() {
		MutexType::Lock lock(_mutex);
		if (_filestream) {
			_filestream.close();
		}
		// std::cout << _filename << std::endl;
		_filestream.open(_filename);
		return !!_filestream;
	}

	std::string FileLogAppender::toXMLString() {
		MutexType::Lock lock(_mutex);

		XMLCreateManager::ptr xmlCreateManager = std::make_shared<XMLCreateManager>();

		xmlCreateManager->AddElementEnd(nullptr, "type", "FileLogAppender");
		xmlCreateManager->AddElementEnd(nullptr, "name", _filename);
		if (_level != LogLevel::UNKNOW) {
			xmlCreateManager->AddElementEnd(nullptr, "level", LogLevel::ToString(_level));
		}
		if (_formatter) {
			xmlCreateManager->AddElementEnd(nullptr, "formatter", _formatter->getPattern());
		}

		std::stringstream ss;
		ss << xmlCreateManager->ConvertXMLToString();
		return ss.str();
	}

	void LogFormatter::Init() {
		// str, format, type
		std::vector<std::tuple<std::string, std::string, int>> vec;
		size_t last_pos = 0;  // 上次位置
		std::string nstr;
		for (size_t i = 0; i < _pattern.size(); ++i) {
			if (_pattern[i] != '%') {  // 消除%
				nstr.append(1, _pattern[i]);
				continue;
			}

			if (i + 1 < _pattern.size() && _pattern[i + 1] == '%') {
				nstr.append(1, '%');
				continue;
			}
			std::string str;
			std::string fmt;
			size_t n = i + 1;
			int fmt_status = 0;  // 记录状态
			size_t fmt_begin = 0;
			while (n < _pattern.size()) {
				if (!isalpha(_pattern[n]) && _pattern[n] != '{' && _pattern[n] != '}') {
					str = _pattern.substr(i + 1, n - i - 1);
					break;
				}
				if (fmt_status == 0) {
					if (_pattern[n] == '{') {
						str = _pattern.substr(i + 1, n - i - 1);
						fmt_status = 1;  // 解析格式
						fmt_begin = n++;
						continue;
					}
				}
				if (fmt_status == 1) {
					if (_pattern[n] == '}') {
						fmt = _pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
						fmt_status = 0;
						++n;
						break;
					}
				}
				++n;
				if (n == _pattern.size()) {
					if (str.empty()) {
						str = _pattern.substr(i + 1);
					}
				}
			}
			if (fmt_status == 0) {
				if (!nstr.empty()) {
					vec.emplace_back(nstr, "", 0);
					nstr.clear();
				}
				// str = _pattern.substr(i + 1, n - i - 1);
				vec.emplace_back(str, fmt, 1);
				i = n - 1;
			}
			else if (fmt_status == 1) {
				std::cout << "pattern parse error: " << _pattern << _pattern.substr(i) << std::endl;
				_is_error = true;
				vec.emplace_back("<<pattern_error>>", fmt, 0);
			}
		}
		if (!nstr.empty()) {
			vec.emplace_back(nstr, "", 0);
		}
		static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)>> s_format_items = {
	#define XX(str, C) \
	            {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}

				XX(m, MessageFormatItem),    // 消息
				XX(p, LevelFormatItem),      // 日志级别
				XX(r, ElapseFormatItem),     // 累计毫秒数
				XX(c, NameFormatItem),       // 日志名称
				XX(t, ThreadIdFormatItem),   // 线程ID
				XX(n, NewLineFormatItem),    // 换行
				XX(d, DateTimeFormatItem),   // 时间
				XX(f, FileNameFormatItem),   // 文件名
				XX(l, LineFormatItem),       // 行号
				XX(T, TableFormatItem),      // Tab
				XX(N, ThreadNameFormatItem)  // 线程名称
	#undef XX
		};
		// 三元组
		for (auto& i : vec) {
			if (std::get<2>(i) == 0) {
				_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
			}
			else {
				auto it = s_format_items.find(std::get<0>(i));
				if (it == s_format_items.end()) {
					_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
					_is_error = true;
				}
				else {
					_items.push_back(it->second(std::get<1>(i)));
				}
			}
			// std::cout << "(" <<std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" <<std::endl;
		}
	}

	std::string LogFormatter::format(const std::shared_ptr<Logger>& logger, LogLevel::Level level, LogEvent::ptr event) {
		std::stringstream ss;
		for (auto& i : _items) {
			i->format(ss, logger, level, event);
		}
		return ss.str();
	}

	std::ostream& LogFormatter::format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
		for (auto& i : _items) {
			std::cout << "LogFormatter::format" << std::endl;
			i->format(ofs, logger, level, event);
		}
		return ofs;
	}

	LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file, int32_t line, uint32_t elapse, uint32_t thread_id,
		uint64_t time, const std::string& thread_name) :
		_level(level),
		_file(file),
		_line(line),
		_elapse(elapse),
		_threadId(thread_id),
		_time(time),
		_logger(logger),
		_thread_name(thread_name) {}

	void LogEvent::format(const char* fmt, ...) {
		va_list al;  // 初始化一个能指向可变长参数列表的指针
		va_start(al, fmt);  // fmt为最后一个指定参数，该宏将al指向可变长参数列表的开头
		format(fmt, al);
		va_end(al);  // 将al置空
	}

	void LogEvent::format(const char* fmt, va_list al) {

		// 计算缓冲区的大小
		va_list al_copy;
		va_copy(al_copy, al);
		int len = vsnprintf(nullptr, 0, fmt, al_copy);
		va_end(al_copy);

		if (len == -1) {
			return; // 错误处理
		}

		// 分配足够的空间来存储格式化后的字符串
		std::vector<char> buf(len + 1);

		// 使用 vsnprintf 填充缓冲区
		vsnprintf(buf.data(), buf.size(), fmt, al);

		// 将格式化结果添加到输出流中
		_ss << std::string(buf.data(), len);
	}

	// ---------------- LogEventWarp -----------------------
	LogEventWarp::LogEventWarp(LogEvent::ptr event) : _event(event) {}

	std::stringstream& LogEventWarp::getSS() {
		return _event->getSS();
	}

	LogEventWarp::~LogEventWarp() {
		_event->getLogger()->log(_event->getLevel(), _event);
	}

}