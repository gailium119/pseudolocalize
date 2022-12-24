// StringRes.hpp --- String Resources
//////////////////////////////////////////////////////////////////////////////
// RisohEditor --- Another free Win32 resource editor
// Copyright (C) 2017-2018 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
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

#pragma once

#include "MByteStreamEx.hpp"
#include "MString.hpp"
#include <map>

//////////////////////////////////////////////////////////////////////////////

struct STRING_ENTRY
{
    WCHAR StringID[128];
    WCHAR StringValue[512];
};

class StringRes
{
public:
    typedef MStringW string_type;
    typedef std::map<WORD, string_type> map_type;
    WORD        m_wName;
    map_type    m_map;

    StringRes() = default;

    bool LoadFromStream(const MByteStreamEx& stream, WORD wName);
    bool SaveToStream(MByteStreamEx& stream, WORD wName);

    string_type Dump(WORD wName,bool pseudolocalize=false);
    string_type Dump(bool pseudolocalize=false);

    map_type& map()
    {
        return m_map;
    }
    const map_type& map() const
    {
        return m_map;
    }

    void GetIdRange(WORD name, WORD& first, WORD& last) const
    {
        first = (name - 1) * 16;
        last = first + 16 - 1;
    }
};
