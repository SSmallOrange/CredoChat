#include "Log/Log.h"
#include "LocalConfig/LocalCfg.h"
#include "LocalConfig/tinyxml2.h"

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

	void Logger::setFormatter(LogFormatter::ptr val) {
		MutexType::Lock lock(_mutex);
		_formatter = val;
		for (auto& i : _appenders) {
			// 通过友元类隔空加锁
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
		XML::XMLDocument doc;
		XML::XMLDeclaration* decl = doc.NewDeclaration(R"(xml version="1.0" encoding="UTF-8")");
		doc.InsertFirstChild(decl);



		node["name"] = _name;
		if (_level != LogLevel::UNKNOW) {
			node["level"] = LogLevel::ToString(_level);
		}
		if (_formatter) {
			node["formatter"] = _formatter->getPattern();
		}

		for (auto& i : _appenders) {
			node["appenders"].push_back(YAML::Load(i->toYamlString()));
		}
		std::stringstream ss;
		ss << node;
		return ss.str();
	}

	// ---------------- LogAppender -----------------------
	void LogAppender::setFormatter(LogFormatter::ptr val) {
		MutexType::Lock lock(_mutex);
		_formatter = val;
	}
}