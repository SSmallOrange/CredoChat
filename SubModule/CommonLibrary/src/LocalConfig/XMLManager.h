#pragma once
#include "tinyxml2.h"

#include <string>
#include <memory>

namespace XML = tinyxml2;

namespace CommonModule {

	class XMLCreateManager {
	public:
		typedef std::shared_ptr<XMLCreateManager> ptr;
		XMLCreateManager() = default;
		~XMLCreateManager() = default;

	public:
		void InitDeclaration(const std::string& str = "");
		void InitDeclaration(XML::XMLDeclaration* decl);

		XML::XMLElement* GetElement(const std::string& strElementText);
		void AddElementFirst(XML::XMLElement* parentElement, std::string strElementText, std::string strContext);
		void AddElementEnd(XML::XMLElement* parentElement, std::string strElementText, std::string strContext);

		std::string ConvertXMLToString();
	private:
		XML::XMLDocument _doc;
	};
}