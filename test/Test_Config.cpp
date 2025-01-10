#include "LocalConfig/LocalCfg.h"
#include "Utils/util.h"

namespace CredoTest {
	void test_Config() {
		std::string configFilePath = CommonModule::CurrentExecPath();
		CommonModule::LocalCfg* config = CommonModule::LocalCfg::GetInstance().get();
		// D:\Data\Project\CredoChat\CredoChat\bin\CredoChat.exe
		// std::string test = "hello world!!"; // "D:\\Data\\Project\\CredoChat\\CredoChat\\bin\\Localcfg.xml"
		if (config->OpenXMLDoc(configFilePath + "\\Localcfg.xml")) {
			std::string str = CommonModule::LocalCfg::GetInstance()->ReadConfigValue<std::string>("IP", "ClientInfo", "Localcfg.xml");
			std::string temp = str;
		}
		system("pause");
		return;
}
}