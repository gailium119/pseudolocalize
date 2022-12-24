// ResToText.hpp --- Dumping Resource
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

#include "MWindowBase.hpp"
#include "MString.hpp"
#include "Res.hpp"

#define MYWM_DELPHI_DFM_B2T (WM_USER + 300)
#define MYWM_TLB_B2T (WM_USER + 301)

HBITMAP CreateBitmapFromIconDx(HICON hIcon, INT width, INT height, BOOL bCursor);
MStringW DumpIconInfo(const BITMAP& bm, BOOL bIcon = TRUE);
MStringW DumpBitmapInfo(HBITMAP hbm);
MStringW DumpGroupIconInfo(const std::vector<BYTE>& data);
MStringW DumpGroupCursorInfo(const std::vector<BYTE>& data);

HBITMAP
CreateBitmapFromIconOrPngDx(HWND hwnd, const EntryBase& entry, BITMAP& bm);

HBITMAP
CreateBitmapFromIconsDx(HWND hwnd, const EntryBase& entry);

HBITMAP
CreateBitmapFromCursorDx(HWND hwnd, const EntryBase& entry, BITMAP& bm);

HBITMAP
CreateBitmapFromCursorsDx(HWND hwnd, const EntryBase& entry);

MString GetLanguageStatement(WORD langid, BOOL bOldStyle, bool pseudolocalize);

//////////////////////////////////////////////////////////////////////////////

class ResToText
{
public:
    typedef std::vector<BYTE> data_type;

    ResToText() : m_hwnd(NULL), m_hwndDialog(NULL), 
                  m_bHumanReadable(TRUE), m_bNoLanguage(FALSE)
    {
    }

    MString DumpEntry(const EntryBase& entry, bool pseudolocalize = false);
    MStringW GetResTypeName(const MIdOrString& type) const;

public:
    HWND m_hwnd;
    HWND m_hwndDialog;
    BOOL m_bHumanReadable;
    BOOL m_bNoLanguage;
    MString m_strFilePrefix;

    MString GetLanguageStatement(WORD langid, bool pseudolocalize=false)
    {
        if (!m_bNoLanguage)
            return ::GetLanguageStatement(langid, TRUE,pseudolocalize) + TEXT("\r\n");
        else
            return TEXT("");
    }

    MString DoCursor(const EntryBase& entry, bool pseudolocalize = false);
    MString DoBitmap(const EntryBase& entry, bool pseudolocalize = false);
    MString DoIcon(const EntryBase& entry, bool pseudolocalize = false);
    MString DoMenu(const EntryBase& entry, bool pseudolocalize = false);
    MString DoToolbar(const EntryBase& entry, bool pseudolocalize = false);
    MString DoDialog(const EntryBase& entry, bool pseudolocalize = false);
    MString DoString(const EntryBase& entry, bool pseudolocalize = false);
    MString DoAccel(const EntryBase& entry, bool pseudolocalize = false);
    MString DoGroupCursor(const EntryBase& entry, bool pseudolocalize = false);
    MString DoGroupIcon(const EntryBase& entry, bool pseudolocalize = false);
    MString DoVersion(const EntryBase& entry, bool pseudolocalize = false);
    MString DoAniCursor(const EntryBase& entry, bool pseudolocalize = false);
    MString DoAniIcon(const EntryBase& entry, bool pseudolocalize = false);
    MString DoText(const EntryBase& entry, bool pseudolocalize = false);
    MString DoManifest(const EntryBase& entry, bool pseudolocalize = false);
    MString DoImage(const EntryBase& entry, bool pseudolocalize = false);
    MString DoMessage(const EntryBase& entry, bool pseudolocalize = false);
    MString DoWave(const EntryBase& entry, bool pseudolocalize = false);
    MString DoAVI(const EntryBase& entry, bool pseudolocalize = false);
    MString DoTypeLib(const EntryBase& entry, bool pseudolocalize = false);
    MString DoDlgInit(const EntryBase& entry, bool pseudolocalize = false);
    MString DoRCData(const EntryBase& entry, bool pseudolocalize = false);
    MString DoUnknown(const EntryBase& entry, bool pseudolocalize = false);
    MString DoFont(const EntryBase& entry, bool pseudolocalize = false);
    MString DoFontDir(const EntryBase& entry, bool pseudolocalize = false);
    MString DoEncodedText(const EntryBase& entry, const MStringW& enc, bool pseudolocalize = false);

    MString DumpName(const MIdOrString& type, const MIdOrString& name);
    MString DumpEscapedName(const MIdOrString& name);

    MString GetEntryFileName(const EntryBase& entry, bool pseudolocalize = false);
};
