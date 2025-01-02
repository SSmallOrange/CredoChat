#pragma once

#include "CommonModule.h"
#include "Utils/singleton.h"

#include <any>
#include <string>
#include <memory>
#include <unordered_map>

namespace tinyxml2 {
	class XMLDocument;
}

namespace CommonModule {
	class COMMONMODULE_API LocalCfg : public Singleton<LocalCfg> {
		typedef std::shared_ptr<tinyxml2::XMLDocument> XMLDocPtr;
	public:
		~LocalCfg() = default;

	private:
		friend class Singleton<LocalCfg>;  // 方便单例调用构造函数进行构造
		LocalCfg();

	public:
		bool OpenXMLDoc(std::string strFileName);

	public:
		// Support: Int Double String Bool
		template<typename T>
		T ReadConfigValue(std::string strType, std::string strMenu, std::string strFileName);

		bool WriteConfigValue(std::any value, std::string strType, std::string strMenu, std::string strFileName);

	private:
		std::any ReadConfigValueAny(std::string strType, std::string strMenu, std::string strFileName);

	private:
		std::unordered_map<std::string, XMLDocPtr> _xmlDocs;
	};
}

