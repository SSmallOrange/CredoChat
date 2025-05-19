#include <iostream>
#include <list>

#include "Utils/StaticInitializerBase.h"
#include "LocalConfig/LocalCfg.h"
#include "Log/LogManager.h"

#include "Net/NetPool.h"

static CommonModule::Logger::ptr g_logger = nullptr;

int main() {

	// main函数前初始化
	// CommonModule::StaticInitializerBase::runAllInitializers();

	std::string configFilePath = CommonModule::CurrentExecPath();
	CommonModule::LocalCfg* config = CommonModule::LocalCfg::GetInstance().get();
	if (config->OpenXMLDoc(configFilePath + "\\Localcfg.xml")) {
	 	std::string str = CommonModule::LocalCfg::GetInstance()->ReadConfigValue<std::string>("IP", "ClientInfo", "Localcfg.xml");
	}
	g_logger = LOG_NAME("root");
	// g_logger->setLevel(CommonModule::LogLevel::INFO);
	CommonModule::Net::NetPool::GetInstance()->SetSessionInfo({ "CredoChat", "www.baidu.com", ""});
	CommonModule::Net::HttpRequest req {"CredoChat", "1234556", "GET", "/", "1.1"};

	for (int i = 0; i < 10; i++) {
		CommonModule::Net::NetPool::GetInstance()->Get_Async("CredoChat", req, [&](CommonModule::Net::HttpResponse response) {
			LOG_INFO(g_logger) << "i : [" << i << "] " << response.strBody;
		});
	}
	LOG_INFO(g_logger) << "Net Request ing";
	std::this_thread::sleep_for(std::chrono::seconds(5));
	LOG_INFO(g_logger) << "Net Request Finsh";

	return 0;
}
