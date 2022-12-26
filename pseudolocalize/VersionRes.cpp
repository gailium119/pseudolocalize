// VersionRes.cpp --- Version Resources
//////////////////////////////////////////////////////////////////////////////
// RisohEditor --- Another free Win32 resource editor
// Copyright (C) 2017-2020 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//////////////////////////////////////////////////////////////////////////////

#include "VersionRes.hpp"
#include "ConstantsDB.hpp"

bool VersionRes::VarsFromStream(Vars& vars, const MByteStreamEx& stream)
{
    Var var;

    stream.ReadDwordAlignment();

    size_t pos0 = stream.pos();

    if (!stream.ReadRaw(var.head) || !stream.ReadSz(var.key))
        return false;

    size_t pos1 = pos0 + var.head.wLength;
    stream.ReadDwordAlignment();

    if (var.head.wValueLength)
    {
        DWORD dwSize = var.head.wValueLength;
        if (var.head.wType == 1)
            dwSize *= 2;
        var.value.resize(dwSize);
        if (!stream.ReadData(&var.value[0], dwSize))
            return false;
    }
    stream.ReadDwordAlignment();

    while (stream.pos() < pos1)
    {
        VarsFromStream(var.vars, stream);
    }

    vars.push_back(var);

    return true;
}

bool VersionRes::LoadFromData(const std::vector<BYTE>& data)
{
    ZeroMemory(&m_fixed, sizeof(m_fixed));

    MByteStreamEx stream(data);
    if (!VarsFromStream(m_vars, stream))
        return false;

    if (m_vars.size() != 1)
        return false;

    Var& var = m_vars[0];
    if (var.key != L"VS_VERSION_INFO")
        return false;

    if (var.value.size() == sizeof(VS_FIXEDFILEINFO))
    {
        CopyMemory(&m_fixed, &var.value[0], var.value.size());
    }

    return true;
}

MStringW
VersionRes::DumpValue(WORD wType, const Var& value, int depth,bool pseudolocalize) const
{
    MStringW ret = MStringW(depth * 4, L' ');
    ret += L"VALUE ";
    ret += mstr_quote(value.key);

    if (value.value.size() >= 2)
    {
        if (wType == 0)
        {
            const WORD *pw = reinterpret_cast<const WORD *>(&value.value[0]);
            WCHAR buf[MAX_PATH];
            for (size_t i = 0; i < value.value.size(); i += 2)
            {
                if (*pw == 1033) {
                    StringCchPrintfW(buf, _countof(buf), L", 0x%04X",1281 );
                     pw++;
                }
                else  StringCchPrintfW(buf, _countof(buf), L", 0x%04X", *pw++);
                ret += buf;
            }
        }
        else
        {
            const WCHAR *pch = reinterpret_cast<const WCHAR *>(&value.value[0]);
            MStringW str(pch, value.value.size() / 2);
            ret += L", ";
            if (str[str.length() - 1] == '\0') str = str.substr(0, str.length() - 1);
            if (pseudolocalize&&_wcsicmp(value.key.c_str() ,L"FileVersion")!=0&& _wcsicmp(value.key.c_str(), L"InternalName") != 0 
                && _wcsicmp(value.key.c_str(), L"OriginalFilename") != 0 && _wcsicmp(value.key.c_str(), L"ProductVersion") != 0) ret += mstr_quote(Pseudo_localize_utf8(str));
            else ret += mstr_quote(str);
        }
    }
    else
    {
        ret += L", \"\"";
    }

    ret += L"\r\n";
    return ret;
}

MStringW
VersionRes::DumpBlock(const Var& var, int depth,bool pseudolocalize) const
{
    MStringW ret;

    ret += MStringW(depth * 4, L' ');
    ret += L"BLOCK \"";
    if (pseudolocalize && _wcsicmp(var.key.c_str(), L"040904B0") == 0)ret+= L"050104B0";
    else ret += var.key;
    ret += L"\"\r\n";
    ret += MStringW(depth * 4, L' ');
    if (false)
        ret += L"BEGIN\r\n";
    else
        ret += L"{\r\n";

    for (auto& item : var.vars)
    {
        if (var.key == L"StringFileInfo")
        {
            ret += DumpBlock(item, depth + 1,pseudolocalize);
        }
        else
        {
            ret += DumpValue(item.head.wType, item, depth + 1,pseudolocalize);
        }
    }

    ret += MStringW(depth * 4, L' ');
    if (false)
        ret += L"END\r\n";
    else
        ret += L"}\r\n";

    return ret;
}

MStringW
VersionRes::Dump(const MIdOrString& name, bool pseudolocalize) const
{
    MStringW ret;
    WCHAR line[MAX_PATH];

    ret += name.str();
    ret += L" VERSIONINFO\r\n";

    StringCchPrintfW(line, _countof(line), 
        L"FILEVERSION     %u, %u, %u, %u\r\n", 
        HIWORD(m_fixed.dwFileVersionMS), 
        LOWORD(m_fixed.dwFileVersionMS), 
        HIWORD(m_fixed.dwFileVersionLS), 
        LOWORD(m_fixed.dwFileVersionLS));
    ret += line;

    StringCchPrintfW(line, _countof(line), 
        L"PRODUCTVERSION  %u, %u, %u, %u\r\n", 
        HIWORD(m_fixed.dwProductVersionMS), 
        LOWORD(m_fixed.dwProductVersionMS), 
        HIWORD(m_fixed.dwProductVersionLS), 
        LOWORD(m_fixed.dwProductVersionLS));
    ret += line;

    StringCchPrintfW(line, _countof(line), L"FILEOS          0x%04lX\r\n", m_fixed.dwFileOS);
    ret += line;

    StringCchPrintfW(line, _countof(line), L"FILETYPE        0x%X\r\n", m_fixed.dwFileType);
    ret += line;

    if (false)
        ret += L"BEGIN\r\n";
    else
        ret += L"{\r\n";

    const std::vector<Var>& vars = m_vars[0].vars;
    for (auto& item : vars)
    {
        ret += DumpBlock(item, 1,pseudolocalize);
    }
    if (false)
        ret += L"END\r\n";
    else
        ret += L"}\r\n";

    return ret;
}
