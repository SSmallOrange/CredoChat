#include <iostream>
#include <list>

#include "LocalConfig/LocalCfg.h"
#include "Log/LogManager.h"
#include "Concurrent/ThreadPool.h"

static CommonModule::Logger::ptr g_logger;

int main() {
	std::string configFilePath = CommonModule::CurrentExecPath();
	CommonModule::LocalCfg* config = CommonModule::LocalCfg::GetInstance().get();
	if (config->OpenXMLDoc(configFilePath + "\\Localcfg.xml")) {
	 	std::string str = CommonModule::LocalCfg::GetInstance()->ReadConfigValue<std::string>("IP", "ClientInfo", "Localcfg.xml");
	}
	g_logger = LOG_NAME("root");


	return 0;
}
