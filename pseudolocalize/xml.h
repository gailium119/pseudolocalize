#pragma once
#include <vector>
#include <string>
#define PathFileExists(lpszPath) (GetFileAttributes(lpszPath) != (DWORD)-1)
#include<stdio.h>
#include<MsXml2.h>
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
HRESULT XmlUnInitialize();
HRESULT XmlInitialize();
HRESULT LoadXML(LPCWSTR pXML, IXMLDOMDocument2** pDocument);

struct MyXmlDoc {
public:
	IXMLDOMDocument2* XmlDoc = NULL;
	HRESULT Load(LPCWSTR path) {
		//this->XmlDoc = (MyXmlDoc*)malloc(sizeof(MyXmlDoc));
		return LoadXML(path, &this->XmlDoc);
	}
	HRESULT Release() {
		if (!this->XmlDoc) return S_FALSE;
		return this->XmlDoc->Release();
	}
	HRESULT Save(LPCWSTR path) {
		if (!this->XmlDoc) return S_FALSE;
		VARIANT varFile;
		VariantInit(&varFile);
		V_BSTR(&varFile) = SysAllocString(path);
		V_VT(&varFile) = VT_BSTR;
		return this->XmlDoc->save(varFile);
	}
};

struct MyXmlElement {
public:
	IXMLDOMNode* XmlNode = NULL;
	std::wstring GetTextDirty() {
		BSTR bstr,bstr2;
		this->XmlNode->get_xml(&bstr);
		this->XmlNode->get_text(&bstr2);
		std::wstring str;
		int len = wcslen(bstr2);
			str = bstr2;
			bool isempty = true;
			for (int cnt = 0; cnt < str.length(); cnt++) {
				if (str[cnt] != ' ' && str[cnt] != '\t' && str[cnt] != '\0' && str[cnt] != '\n' && str[cnt] != '\r' && str[cnt] != ' ') {
					isempty = false;
				}
			}
			if (str.length() == 0)isempty = true;
			if (isempty)str = L"";
			else {
				str = bstr;
				str = str.substr(str.find(L'>') + 1);
				str = str.substr(0, str.rfind(L'<'));
				wprintf(L"%ls\n", str.c_str());
			}
		SysFreeString(bstr);
		SysFreeString(bstr2);
		return str;
	}
	std::wstring GetText() {
		BSTR bstr;
		this->XmlNode->get_text(&bstr);
		std::wstring str=bstr;
		SysFreeString(bstr);
		return str;
	}
	HRESULT SetText(std::wstring text) {
		BSTR bstr=SysAllocString(text.c_str());
		HRESULT hr=this->XmlNode->put_text(bstr);
		SysFreeString(bstr);
		return hr;
	}
	std::wstring GetAttr(std::wstring attrname) {
		IXMLDOMNamedNodeMap* NodeMap = NULL;
		IXMLDOMNode* IDNode = NULL;
		BSTR bstr;
		std::wstring ret;
		HRESULT hr = this->XmlNode->get_attributes(&NodeMap);
		if (hr != S_OK) {
			NodeMap->Release();
			this->XmlNode->Release();
			return L"";
		}
		BSTR attrbstr = ::SysAllocString(attrname.c_str());
		HRESULT result = NodeMap->getNamedItem(attrbstr, &IDNode);
		SysFreeString(attrbstr);
		if (result == S_OK) {
			//
			// Dump id => 1
			//

			if (IDNode->get_text(&bstr) == S_OK) {
			
				ret = bstr;
				SysFreeString(bstr);
			}
			IDNode->Release();
			NodeMap->Release();
		}
		else {
			if (IDNode)IDNode->Release();
			if (NodeMap)NodeMap->Release();
		}
		return ret;
	}
	HRESULT SetAttr(std::wstring attrname,std::wstring value) {
		IXMLDOMNamedNodeMap* NodeMap = NULL;
		IXMLDOMNode* IDNode = NULL;
		BSTR bstr=SysAllocString(value.c_str());
		HRESULT hr = this->XmlNode->get_attributes(&NodeMap);
		if (hr != S_OK) {
			NodeMap->Release();
			this->XmlNode->Release();
			return hr;
		}
		BSTR attrbstr = ::SysAllocString(attrname.c_str());
		HRESULT result = NodeMap->getNamedItem(attrbstr, &IDNode);
		SysFreeString(attrbstr);
		if (result == S_OK) {
			//
			// Dump id => 1
			//

			if (IDNode->put_text(bstr) == S_OK) {

				SysFreeString(bstr);
			}
			IDNode->Release();
			NodeMap->Release();
		}
		else {
			if (IDNode)IDNode->Release();
			if (NodeMap)NodeMap->Release();
		}
		return result;
	}

	HRESULT Release() {
		if (!this->XmlNode) return S_FALSE;
		return this->XmlNode->Release();
	}
};
struct MyXmlElementTable {
public:
	IXMLDOMNodeList* XmlNodeList = NULL;
	HRESULT SelectFromDocument(MyXmlDoc doc, LPCWSTR expr) {
		return doc.XmlDoc->selectNodes((WCHAR*)expr, &this->XmlNodeList);
	}
	HRESULT Length(long* result) {
		return this->XmlNodeList->get_length(result);
	}
	HRESULT SelectFromElement(MyXmlElement element, LPCWSTR expr) {
		return element.XmlNode->selectNodes((WCHAR*)expr, &this->XmlNodeList);
	}
	std::vector<MyXmlElement> GetElements();

	HRESULT Release() {
		"XmlNodeList";
		if (!this->XmlNodeList) return S_FALSE;
		return this->XmlNodeList->Release();
	}
};