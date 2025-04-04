#include "XMLManager.h"

namespace CommonModule {
	void XMLCreateManager::InitDeclaration(const std::string& str /* = "" */) {
		XML::XMLDeclaration* decl = nullptr;
		if (str.empty()) {
			decl = _doc.NewDeclaration(R"(xml version="1.0" encoding="UTF-8")");
		}
		else {
			decl = _doc.NewDeclaration(str.c_str());
		}
		_doc.InsertFirstChild(decl);
	}

	void XMLCreateManager::InitDeclaration(XML::XMLDeclaration* decl) {
		_doc.InsertFirstChild(decl);
	}

	XML::XMLElement* XMLCreateManager::GetElement(const std::string& strElementText) {
		return _doc.NewElement(strElementText.c_str());
	}

	void XMLCreateManager::AddElementFirst(XML::XMLElement* parentElement, std::string strElementText, std::string strContext) {
		if (parentElement == nullptr) {
			_doc.InsertFirstChild(GetElement(strElementText));
		}
		else {
			parentElement->InsertFirstChild(GetElement(strElementText));
		}
	}

	void XMLCreateManager::AddElementEnd(XML::XMLElement* parentElement, std::string strElementText, std::string strContext) {
		XML::XMLElement* element = GetElement(strElementText);
		if (parentElement == nullptr) {
			_doc.InsertEndChild(element);
		}
		else {
			parentElement->InsertEndChild(element);
		}
		element->SetText(strContext.c_str());
	}

	std::string XMLCreateManager::ConvertXMLToString() {
		XML::XMLPrinter printer;
		_doc.Print(&printer); // 将 XML 内容打印到 XMLPrinter 中
		return std::string(printer.CStr()); // 获取生成的 XML 字符串
	}

}