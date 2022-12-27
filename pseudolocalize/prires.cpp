// prirecompile.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <string>
#include <windows.h>
#include <iostream>
#include"prires.h"
#include<MrmResourceIndexer.h>
#include<algorithm>
#pragma comment(lib,"Mrmsupport.lib")
#include"xml.h"
#include "pseudo.h"
struct MrmCfg {
	std::wstring familyname;
	std::wstring lang;
	bool isautomerge;
};
struct MrmResBase
{
	std::wstring uri;
	int index;
	std::wstring type;
	std::wstring lang;
};
struct MrmResString : MrmResBase {
	std::wstring value;
};
struct MrmResEmbed : MrmResBase {
	BYTE* value;
	size_t size;
};
struct MrmResPath :MrmResBase {
	std::wstring value;
};
std::string ws2s(const std::wstring& ws)
{
	size_t convertedChars = 0;
	const wchar_t* wcs = ws.c_str();
	size_t dByteNum = sizeof(wchar_t) * ws.size() + 1;

	char* dest = new char[dByteNum];
	wcstombs_s(&convertedChars, dest, dByteNum, wcs, _TRUNCATE);
	std::string result = dest;
	delete[] dest;
	return result;
}
MrmResBase* GetRes(MyXmlElement element, MrmCfg cfg) {
	MyXmlElementTable table;
	table.SelectFromElement(element, L"Candidate");
	MrmResBase* ret;
	if (table.GetElements().size() == 0) {
		// No value
		MrmResPath* res = new MrmResPath();
		res->type = L"Path";
		res->value = L"";
		if (cfg.lang.length() == 0) res->lang = L"qps-plocm";
		else res->lang = L"";
		ret = res;
	}
	else {
		auto emts = table.GetElements();
		MyXmlElement subelement = emts[0];
		std::wstring type = subelement.GetAttr(L"type");
		//	_CrtDumpMemoryLeaks();

		if (_wcsicmp(type.c_str(), L"String") == 0) {
			MyXmlElementTable subtable;
			subtable.SelectFromElement(subelement, L"Value");
			MyXmlElement e = subtable.GetElements()[0];
			MrmResString* res = new MrmResString();
			res->lang = cfg.lang;
			res->type = L"String";
			res->value = e.GetText();
			e.Release();
			subtable.Release();
			ret = res;
		}
		else if (_wcsicmp(type.c_str(), L"Path") == 0) {
			MyXmlElementTable subtable;
			subtable.SelectFromElement(subelement, L"Value");
			MyXmlElement e = subtable.GetElements()[0];

			MrmResPath* res = new MrmResPath();
			res->lang = cfg.lang;
			res->type = L"Path";
			res->value = e.GetText();
			e.Release();
			subtable.Release();
			ret = res;
		}
		else if (_wcsicmp(type.c_str(), L"EmbeddedData") == 0) {
			MyXmlElementTable subtable;
			subtable.SelectFromElement(subelement, L"Base64Value");
			MyXmlElement e = subtable.GetElements()[0];

			MrmResEmbed* res = new MrmResEmbed();
			res->lang = cfg.lang;
			res->type = L"EmbeddedData";

			std::wstring b64w = e.GetText();
			e.Release();
			auto b64 = ws2s(b64w);
			auto size = base64_decode((BYTE*)b64.c_str(), NULL, b64.length());
			res->value = (BYTE*)malloc(size * sizeof(BYTE));
			base64_decode((BYTE*)b64.c_str(), res->value, b64.length());
			res->size = size;
			subtable.Release();
			ret = res;
		}
		else ret = new MrmResBase();
		subelement.Release();
	}
	ret->uri = element.GetAttr(L"uri");
	ret->index = _wtoi(element.GetAttr(L"index").c_str());
	table.Release();
	return ret;
}
MrmCfg GetCfg(MyXmlDoc doc) {
	MyXmlElementTable table;
	table.SelectFromDocument(doc, L"//ResourceMap");
	MrmCfg cfg;
	cfg.familyname = table.GetElements()[0].GetAttr(L"name");
	table.Release();
	table.SelectFromDocument(doc, L"//AutoMerge");
	cfg.isautomerge = _wcsicmp(table.GetElements()[0].GetText().c_str(), L"true") == 0;
	table.SelectFromDocument(doc, L"//Qualifiers/Qualifier");
	auto elements = table.GetElements();
	for (int i = 0; i < elements.size(); i++) {
		if (_wcsicmp(elements[i].GetAttr(L"name").c_str(), L"Language") == 0)cfg.lang = elements[i].GetAttr(L"value");
		elements[i].Release();
	}
	//if (cfg.isautomerge) cfg.lang = L"";
	table.Release();
	return cfg;
}
bool MrmIsBiggerThan(MrmResBase* v1, MrmResBase* v2) {
	return v1->index > v2->index;
}
bool MrmIsSmallerThan(MrmResBase* v1, MrmResBase* v2) {
	return v1->index < v2->index;
}
HRESULT AddRes(MrmResBase* res, MrmResourceIndexerHandle indexer, bool pseudolocalize = false) {
	HRESULT hr = S_OK;
	std::wstring qualifier = L"language-";
	if (res->lang.empty())qualifier = L"";
	else qualifier += res->lang;
	if (_wcsicmp(res->type.c_str(), L"String") == 0) {
		MrmResString* resr = (MrmResString*)res;
		if(pseudolocalize) hr = MrmIndexString(indexer, resr->uri.c_str(),Pseudo_localize_utf8(resr->value).c_str(), qualifier.c_str());
		else  hr = MrmIndexString(indexer, resr->uri.c_str(), resr->value.c_str(), qualifier.c_str());
		delete resr;
	}
	else if (_wcsicmp(res->type.c_str(), L"Path") == 0) {
		MrmResPath* resr = (MrmResPath*)res;
		hr = MrmIndexFile(indexer, resr->uri.c_str(), resr->value.c_str(), qualifier.c_str());
		delete resr;
	}
	else if (_wcsicmp(res->type.c_str(), L"EmbeddedData") == 0) {
		MrmResEmbed* resr = (MrmResEmbed*)res;
		hr = MrmIndexEmbeddedData(indexer, resr->uri.c_str(), resr->value, resr->size, qualifier.c_str());
		delete[]resr->value;
		delete resr;
	}
	return hr;
}
HRESULT CreatePriFromXml(LPCWSTR xmlpath,bool pseudolocalize=false) {
    MyXmlDoc doc;
    doc.Load(xmlpath);
    MyXmlElementTable table;
    table.SelectFromDocument(doc, L"//NamedResource");
    auto elements = table.GetElements();
    MrmCfg cfg = GetCfg(doc);
	if (pseudolocalize) cfg.lang = L"QPS-PLOC";
    std::vector<MrmResBase*> resS;
    for (int i = 0; i < elements.size(); i++) {
        resS.push_back(GetRes(elements[i], cfg));
        elements[i].Release();
        wprintf(L"%d\n", i);
    }
    std::sort(resS.begin(), resS.end(), MrmIsBiggerThan);
    MrmResourceIndexerHandle indexer;
    HRESULT hr;
    hr = MrmCreateResourceIndexerWithFlags(cfg.familyname.c_str(), cfg.familyname.c_str(), MrmPlatformVersion::MrmPlatformVersion_Windows10_0_0_0, L"", MrmIndexerFlagsNone, &indexer);
    //MrmCreateResourceIndexerFromPreviousPriFile(cfg.familyname.c_str(),  MrmPlatformVersion::MrmPlatformVersion_Windows10_0_0_0, L"",L"D:\\tmp\\pri\\windows.ui.settingsappthreshold.qps-ploc.pri", &indexer);
    for (int i = 0; i < resS.size(); i++) {

        wprintf(L"%d,0x%x\n", i, AddRes(resS[i], indexer,pseudolocalize));

    }
    hr = (::MrmCreateResourceFile(indexer, MrmPackagingModeAutoSplit, MrmPackagingOptionsNone, L".\\"));
    // MrmCreateResourceFileWithChecksum(indexer, MrmPackagingModeAutoSplit, MrmPackagingOptionsNone,1919810 ,L".\\");
    (::MrmDestroyIndexerAndMessages(indexer));
    table.Release();
    doc.Release();
    return hr;
}
HRESULT CreatePri(LPCWSTR pripath, bool pseudolocalize) {
	auto hr=MrmDumpPriFile(pripath, nullptr, MrmDumpType::MrmDumpType_Detailed, L"tmp.xml");
	hr = CreatePriFromXml(L"tmp.xml", pseudolocalize);
	CopyFileW(L".\\resources.language-qps-ploc.pri", pripath, 0);
	DeleteFileW(L".\\resources.language-qps-ploc.pri");
	DeleteFileW(L".\\resources.pri");
	DeleteFileW(L".\\tmp.xml");
	return hr;
}