#include"xml.h"

HRESULT LoadXML(LPCWSTR pXML, IXMLDOMDocument2** pDocument)
{
	HRESULT hr;
	IXMLDOMDocument2* pXMLDoc = NULL;
	IXMLDOMParseError* pObjError = NULL;
	BSTR bstr = NULL;
	VARIANT_BOOL status;
	VARIANT vSrc;
	long     length = 0;
	TCHAR* buffer = NULL;
	int UseShortPath = 0;
	std::wstring str = L"\\\\?\\";
	if (!PathFileExists(pXML)) {

		str += pXML;
		length = GetShortPathNameW(str.c_str(), NULL, 0);
		if (length == 0) return 0x80070002;

		buffer = (wchar_t*)malloc(length * sizeof(wchar_t));

		length = GetShortPathName(str.c_str(), buffer, length);
		if (length == 0) return 0x80070002;
		UseShortPath = 1;
		str = buffer;
		free(buffer);
		str = str.substr(4, str.length() - 4);
	}
	hr = CoCreateInstance(_uuidof(DOMDocument),
		NULL,
		CLSCTX_INPROC_SERVER,
		__uuidof(IXMLDOMDocument2),
		(void**)&pXMLDoc);
	if (FAILED(hr)) {
		printf("Failed to CoCreate an instance of an XML DOM/n");
		printf("Error code: %x/n", hr);
		goto clean;
	}

	hr = pXMLDoc->put_async(VARIANT_FALSE);
	if (FAILED(hr)) {
		printf("Failed to set async property/n");
		goto clean;
	}

	hr = pXMLDoc->put_validateOnParse(VARIANT_FALSE);
	if (FAILED(hr)) {
		printf("Failed to set validateOnParse/n");
		goto clean;
	}

	hr = pXMLDoc->put_resolveExternals(VARIANT_FALSE);
	if (FAILED(hr)) {
		printf("Failed to disable resolving externals./n");
		goto clean;
	}

	VariantInit(&vSrc);
	if (UseShortPath == 1) {
		V_BSTR(&vSrc) = SysAllocString(str.c_str());
	}
	else {
		V_BSTR(&vSrc) = SysAllocString(pXML);
	}
	V_VT(&vSrc) = VT_BSTR;

	hr = pXMLDoc->load(vSrc, &status);

	if (status != VARIANT_TRUE) {
		hr = pXMLDoc->get_parseError(&pObjError);

		wprintf_s(L"My Error: 0x%X", hr);
		hr = pObjError->get_reason(&bstr);
		goto clean;
	}

clean:
	if (bstr)
		SysFreeString(bstr);
	if (&vSrc)
		VariantClear(&vSrc);
	if (pObjError)
		pObjError->Release();
	if (pDocument == NULL) {
		void* data = malloc(sizeof(pXMLDoc));
		pDocument = (IXMLDOMDocument2**)&data;

	}
	*pDocument = pXMLDoc;
	return hr;
}

HRESULT XmlInitialize() {

	return CoInitialize(NULL);
}
HRESULT XmlUnInitialize() {
	CoUninitialize();
	return 0;
}
std::vector<MyXmlElement> MyXmlElementTable::GetElements() {
	std::vector<MyXmlElement> vec;
	vec.clear();
	long length;
	this->Length(&length);
	IXMLDOMNode* pNode = NULL;
	MyXmlElement tmpelement;
	for (int i = 0; i < length; i++) {
		vec.push_back(tmpelement);
		if (this->XmlNodeList->get_item(i, &vec[i].XmlNode) != S_OK) {
			vec.pop_back();
		}
	}
	return vec;
}