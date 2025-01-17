// #include "mainwindow.h"
// #include "Utils/util.h"
// #include "LocalConfig/LocalCfg.h"

#include "LocalConfig/LocalCfg.h"
#include "Log/LogManager.h"
#include "Utils/util.h"

// #include <QFile>
// #include <QApplication>

int main(int argc, char* argv[])
{
	// freopen("CONOUT$", "w", stdout);

	std::string configFilePath = CommonModule::CurrentExecPath();
	CommonModule::LocalCfg* config = CommonModule::LocalCfg::GetInstance().get();
	// D:\Data\Project\CredoChat\CredoChat\bin\CredoChat.exe
	// std::string test = "hello world!!"; // "D:\\Data\\Project\\CredoChat\\CredoChat\\bin\\Localcfg.xml"
	if (config->OpenXMLDoc(configFilePath + "\\Localcfg.xml")) {
		std::string str = CommonModule::LocalCfg::GetInstance()->ReadConfigValue<std::string>("IP", "ClientInfo", "Localcfg.xml");
	}

	static CommonModule::Logger::ptr g_logger = LOG_NAME("root");

	LOG_INFO(g_logger) << "Hello World";

	system("pause");
	return 0;
	//     QApplication a(argc, argv);
	// 
	//     MainWindow w;
	// 
	//     QFile qss(":/style/stylesheet.qss");
	//     if (qss.open(QFile::ReadOnly)) {
	//         QString style = QLatin1String(qss.readAll());
	//         a.setStyleSheet(style);
	//         qDebug("%s", style.toStdString().c_str());
	//         qss.close();
	//     } else {
	//         qDebug("qss error");
	//     }
	// 
	//     w.show();
	//     return a.exec();
}
