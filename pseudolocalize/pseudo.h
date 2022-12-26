#include <string>
#pragma once
std::wstring Pseudo_localize(std::wstring str, bool genid = true, bool wraparound = true, bool extend = true);
std::wstring Pseudo_localize_utf8(std::wstring str, bool genid = true, bool wraparound = true, bool extend = true,bool genidwithnewline=true);
void ReadData1(std::wstring path);
void Pseudo_file(std::wstring path);