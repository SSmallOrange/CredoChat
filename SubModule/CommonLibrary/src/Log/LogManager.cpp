#include "LogManager.h"
#include "LocalConfig\LocalCfg.h"

namespace CommonModule {
	LoggerManager::LoggerManager() {
		_root.reset(new Logger);
		// m_root->addAppender(LogAppender::ptr(new FileLogAppender));
		std::string strLogFileName = CommonModule::LocalCfg::GetInstance()->ReadConfigValue<std::string>("IP", "ClientInfo", "Localcfg.xml");

		m_loggers[m_root->m_name] = m_root;

		init();
	}

	Logger::ptr LoggerManager::getLogger(const std::string& name) {
		MutexType::Lock lock(m_mutex);
		auto it = m_loggers.find(name);
		if (it != m_loggers.end()) {
			return it->second;
		}
		Logger::ptr logger(new Logger(name));
		logger->m_root = m_root;
		m_loggers[name] = logger;
		return logger;
	}

}
