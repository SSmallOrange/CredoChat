#include "LogManager.h"
#include "LocalConfig\LocalCfg.h"
#include "LocalConfig\XMLManager.h"

namespace CommonModule {
	LoggerManager::LoggerManager() {
		_root.reset(new Logger);  // _name = root
		// m_root->addAppender(LogAppender::ptr(new FileLogAppender));
		std::string strLogFileName = CommonModule::LocalCfg::GetInstance()->ReadConfigValue<std::string>("LogFileName", "LogConfig", "Localcfg.xml");
		std::string strLogFilePath = CommonModule::LocalCfg::GetInstance()->ReadConfigValue<std::string>("LogFilePath", "LogConfig", "Localcfg.xml");

		std::string strLogFile = strLogFilePath + '/' + strLogFileName;

		_root->addAppender(LogAppender::ptr(new FileLogAppender(strLogFile)));

		_loggers[_root->getName()] = _root;

		init();
	}

	Logger::ptr LoggerManager::getLogger(const std::string& name) {
		MutexType::Lock lock(_mutex);
		auto it = _loggers.find(name);
		if (it != _loggers.end()) {
			return it->second;
		}
		Logger::ptr logger(new Logger(name));
		logger->setLogger(_root);
		_loggers[name] = logger;
		return logger;
	}

	void LoggerManager::init() {

	}

	std::string LoggerManager::toXMLString() {
		MutexType::Lock lock(_mutex);

		XMLCreateManager::ptr xmlCreateManager = std::make_shared<XMLCreateManager>();

		for (auto& i : _loggers) {
			xmlCreateManager->AddElementEnd(nullptr, i.second->getName(), i.second->toXMLString());
		}
		std::stringstream ss;
		ss << xmlCreateManager->ConvertXMLToString();
		return ss.str();
	}
}
