#include "LocalCfg.h"
#include "tinyxml2.h"

#include <regex>
#include <filesystem>

namespace XML = tinyxml2;

namespace CommonModule {

	LocalCfg::LocalCfg() {
		XML::XMLDocument doc;
	}

	bool isInteger(const std::string& str) {
		std::regex integerRegex("^[+-]?\\d+$");
		return std::regex_match(str, integerRegex);
	}

	bool isFloat(const std::string& str) {
		std::regex floatRegex("^[+-]?([0-9]*[.])?[0-9]+$");
		return std::regex_match(str, floatRegex);
	}

	bool isBool(const std::string& str) {
		return isInteger(str) || str == "true" || str == "false";
	}

	bool isTrue(const std::string& str) {
		if (isBool(str)) {
			if (isInteger(str)) {
				return str == "0" ? false : true;
			}
			if (str == "true" || str == "false") {
				return str == "true" ? true : false;
			}
		}
		return false;
	}

	bool LocalCfg::OpenXMLDoc(std::string strFilePath)
	{
		if (!strFilePath.empty()) {
			std::string fileName = std::filesystem::path(strFilePath).filename().string();
			if (_xmlDocs.find(fileName) != _xmlDocs.end()) {
				return true;
			}
			XMLDocPtr doc = std::make_shared<XML::XMLDocument>();
			XML::XMLError eResult = doc->LoadFile(strFilePath.c_str());
			if (eResult != XML::XML_SUCCESS) {
				return false;
			}
			_xmlDocs[fileName] = doc;
			return true;
		}
		return false;
	}

	template<typename T>
	T LocalCfg::ReadConfigValue(std::string strType, std::string strMenu, std::string strFileName) {
		std::any res = std::move(ReadConfigValue(strType, strMenu, strFileName));
		return std::any_cast<T>(res);
	}
	template<>
	std::string LocalCfg::ReadConfigValue(std::string strType, std::string strMenu, std::string strFileName) {
		std::any res = std::move(ReadConfigValueAny(strType, strMenu, strFileName));
		if (res.has_value()) {
			return std::any_cast<std::string>(res);
		}
		return "";
	}

	template<>
	int LocalCfg::ReadConfigValue(std::string strType, std::string strMenu, std::string strFileName) {
		std::any res = std::move(ReadConfigValueAny(strType, strMenu, strFileName));
		if (res.has_value()) {
			return std::any_cast<int>(res);
		}
		return -1048576;
	}

	template<>
	double LocalCfg::ReadConfigValue(std::string strType, std::string strMenu, std::string strFileName) {
		std::any res = std::move(ReadConfigValueAny(strType, strMenu, strFileName));
		if (res.has_value()) {
			return std::any_cast<double>(res);
		}
		return -1048576;
	}

	std::any LocalCfg::ReadConfigValueAny(std::string strType, std::string strMenu, std::string strFileName)
	{
		if (_xmlDocs.find(strFileName) == _xmlDocs.end()) {
			return std::any{};
		}
		XMLDocPtr doc = _xmlDocs[strFileName];


		XML::XMLElement* root = doc->RootElement();
		if (!root) {
			return std::any{};  // 没有找到根节点
		}

		const char* temp = root->Name();

		XML::XMLElement* menuElement = root->FirstChildElement(strMenu.c_str());
		if (menuElement == nullptr) {
			return std::any{};  // 没有找到类型节点
		}

		XML::XMLElement* typeElement = menuElement->FirstChildElement(strType.c_str());
		if (typeElement == nullptr) {
			return std::any{};  // 没有找到菜单节点
		}

		const char* value = typeElement->GetText();
		if (value == nullptr) {
			return std::any{};  // 没有找到文本值
		}

		if (isInteger(value)) {
			return std::any(atoi(value));
		}
		if (isFloat(value)) {
			return std::any(atof(value));
		}
		if (isBool(value)) {
			return std::any(isTrue(value));
		}

		return std::any(std::string(value));
	}

	bool LocalCfg::WriteConfigValue(std::any value, std::string strType, std::string strMenu, std::string strFileName)
	{
		if (_xmlDocs.find(strFileName) == _xmlDocs.end()) {
			return false;
		}
		XMLDocPtr doc = _xmlDocs[strFileName];


		XML::XMLElement* root = doc->RootElement();
		if (!root) {
			return false;  // 没有找到根节点
		}

		XML::XMLElement* menuElement = root->FirstChildElement(strMenu.c_str());
		if (menuElement == nullptr) {
			return false;  // 没有找到类型节点
		}

		XML::XMLElement* targetElement = menuElement->FirstChildElement(strMenu.c_str());
		if (targetElement == nullptr) {
			return false;  // 没有找到菜单节点
		}

		// 根据不同类型处理写入值
		if (value.type() == typeid(int)) {
			int intValue = std::any_cast<int>(value);
			targetElement->SetText(intValue);
		}
		else if (value.type() == typeid(bool)) {
			bool boolValue = std::any_cast<bool>(value);
			targetElement->SetText(boolValue ? "true" : "false");
		}
		else if (value.type() == typeid(std::string)) {
			std::string stringValue = std::any_cast<std::string>(value);
			targetElement->SetText(stringValue.c_str());
		}
		else if (value.type() == typeid(float)) {
			float floatValue = std::any_cast<float>(value);
			targetElement->SetText(floatValue);
		}

		return false;
	}
}
