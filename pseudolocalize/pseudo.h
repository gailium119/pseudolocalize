#include <string>
#include <vector>
#include <winnt.h>
#pragma once
std::wstring Pseudo_localize(std::wstring str, bool genid = true, bool wraparound = true, bool extend = true, bool genidwithnewline = true, bool skipempty = false);
std::wstring Pseudo_localize_utf8(std::wstring str, bool genid = true, bool wraparound = true, bool extend = true,bool genidwithnewline=true,bool skipempty=false);
void ReadData1(std::wstring path);
void Pseudo_file(std::wstring path);
void Pseudo_xml(LPCWSTR xmlpath, std::vector<std::wstring> textxpaths, std::vector<std::pair<std::wstring, std::wstring>>attrxpaths, bool dirty = true);
void Pseudo_inf(std::wstring path);
void Pseudo_ini(std::wstring path);
void Pseudo_mfl(std::wstring path);
void Pseudo_psd1(std::wstring path);
void Pseudo_resx(LPCWSTR path);