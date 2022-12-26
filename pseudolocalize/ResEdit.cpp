#define USE_GLOBALS
#include <iostream>
#include "ToolbarRes.hpp"

#include <initguid.h>
#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <dlgs.h>
#include <tchar.h>
#include <commctrl.h>
#include <commdlg.h>
#include <mbstring.h>
#include <mmsystem.h>
#include <process.h>
#include <uxtheme.h>
#include <urlmon.h>
#include <wininet.h>


#include <algorithm>    // for std::sort
#include <string>       // for std::string, std::wstring
#include <cassert>      // for assert macro
#include <vector>       // for std::vector
#include <map>          // for std::map
#include <cstdio>
#include <clocale>
#include <strsafe.h>
#include "Res.hpp"
#include "ResToText.hpp"
#include "StringRes.hpp"
MStringW m_commands;
MStringW m_load_options;
MStringW m_save_options;
ConstantsDB g_db;           // constants database
RisohSettings g_settings;   // settings
EntrySet g_res;
WCHAR       m_szFile[MAX_PATH];             // the file location
WCHAR       m_szResourceH[MAX_PATH];        // the resource.h file location
BOOL UnloadResourceH()
{
    // delete all the macro IDs
    auto it = g_db.m_map.find(L"RESOURCE.ID");
    if (it != g_db.m_map.end())
    {
        it->second.clear();
    }

    // reset the settings of the resource.h file
    g_settings.AddIDC_STATIC();
    g_settings.id_map.clear();
    g_settings.added_ids.clear();
    g_settings.removed_ids.clear();
    m_szResourceH[0] = 0;


    // select the selected entry
    auto entry = g_res.get_entry();

    return TRUE;
}

BOOL DoLoadFile(LPCWSTR pszFileName, DWORD nFilterIndex, BOOL bForceDecompress)
{
    WCHAR szPath[MAX_PATH], szResolvedPath[MAX_PATH], * pchPart;

    enum LoadFilterIndex        // see also: IDS_EXERESRCFILTER
    {
        LFI_NONE = 0,
        LFI_EXECUTABLE = 1,
        LFI_RES = 2,
        LFI_RC = 3,
        LFI_ALL = 4
    };

    if (nFilterIndex == LFI_ALL)
        nFilterIndex = LFI_NONE;


    GetFullPathNameW(pszFileName, _countof(szPath), szPath, &pchPart);


    // find the dot extension
    LPWSTR pch = wcsrchr(szPath, L'.');
    if (nFilterIndex == LFI_NONE || nFilterIndex == LFI_EXECUTABLE)
    {
        if (pch && lstrcmpiW(pch, L".res") == 0)
            nFilterIndex = LFI_RES;
        else if (pch && lstrcmpiW(pch, L".rc") == 0)
            nFilterIndex = LFI_RC;
    }

    if (nFilterIndex == LFI_RES)     // .res files
    {
        // reload the resource.h if necessary
        UnloadResourceH();

        // do import to the res variable
        EntrySet res;
        if (!res.import_res(szPath))
        {
            return FALSE;
        }

        // load it now

            // renewal
        g_res.delete_all();
        g_res.merge(res);

        // clean up
        res.delete_all();


        return TRUE;
    }


    LPWSTR pszPath = szPath;        // the real path

    // check whether it was compressed
    MStringW strToOpen = pszPath;

    // load an executable files
    HMODULE hMod = LoadLibraryExW(strToOpen.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE |
        LOAD_LIBRARY_AS_IMAGE_RESOURCE |
        LOAD_WITH_ALTERED_SEARCH_PATH);
    if (hMod == NULL)
    {
        // replace the path
#ifdef _WIN64
        mstr_replace_all(strToOpen,
            L"C:\\Program Files\\",
            L"C:\\Program Files (x86)\\");
#else
        mstr_replace_all(strToOpen,
            L"C:\\Program Files (x86)\\",
            L"C:\\Program Files\\");
#endif

        // retry to load
        hMod = LoadLibraryExW(strToOpen.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE |
            LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_WITH_ALTERED_SEARCH_PATH);
        if (hMod)
        {
            // ok, succeeded
            StringCchCopy(pszPath, _countof(szPath), strToOpen.c_str());
        }
        else
        {
            // retry again
            hMod = LoadLibraryW(strToOpen.c_str());
            if (hMod == NULL)
            {

                return FALSE;       // failure
            }
        }
    }

    // unload the resource.h file
    UnloadResourceH();
    g_res.delete_all();
    g_res.from_res(hMod);
    // free the executable
    FreeLibrary(hMod);

    // update the file info (using the real path)



    return TRUE;    // success
}
BOOL ParseCommandLine(HWND hwnd, INT argc, WCHAR** targv)
{
    LPWSTR file = NULL;
    BOOL bNoGUI = FALSE;
    m_commands.clear();
    for (INT iarg = 1; iarg < argc; ++iarg)
    {
        LPWSTR arg = targv[iarg];

        if (lstrcmpiW(arg, L"-load") == 0 ||
            lstrcmpiW(arg, L"--load") == 0)
        {
            bNoGUI = TRUE;
            arg = targv[++iarg];
            m_commands += L"load:";
            m_commands += arg;
            m_commands += L"\n";
            continue;
        }
        if (lstrcmpiW(arg, L"-load-options") == 0 ||
            lstrcmpiW(arg, L"--load-options") == 0)
        {
            arg = targv[++iarg];
            m_load_options = arg;
            continue;
        }

        if (lstrcmpiW(arg, L"-save") == 0 ||
            lstrcmpiW(arg, L"--save") == 0)
        {
            bNoGUI = TRUE;
            arg = targv[++iarg];
            m_commands += L"save:";
            m_commands += arg;
            m_commands += L"\n";
        }
        if (lstrcmpiW(arg, L"-save-options") == 0 ||
            lstrcmpiW(arg, L"--save-options") == 0)
        {
            arg = targv[++iarg];
            m_save_options = arg;
            continue;
        }

        if (PathFileExistsW(arg))
        {
            if (!file)
                file = arg;
            continue;
        }
    }

    return true;
}
BOOL DoExtract(const EntryBase* entry, BOOL bExporting, bool pseudolocalize=false)
{
   
    ResToText res2text;

    if (bExporting && g_settings.bStoreToResFolder)
    {
        // add "res\\" to the prefix if necessary
        res2text.m_strFilePrefix = L"res\\";
    }

    // get the entry file name
    MString filename = res2text.GetEntryFileName(*entry);
    if (filename.empty())
        return TRUE;        // no need to extract

    //MessageBox(NULL, filename.c_str(), NULL, 0);

    if (entry->m_type.is_int())
    {
        WORD wType = entry->m_type.m_id;
        if (wType == (WORD)(UINT_PTR)RT_CURSOR)
        {
            // No output file
            return TRUE;
        }
        if (wType == (WORD)(UINT_PTR)RT_BITMAP)
        {
            return PackedDIB_Extract(filename.c_str(), &(*entry)[0], entry->size(), FALSE);
        }
        if (wType == (WORD)(UINT_PTR)RT_ICON)
        {
            // No output file
            return TRUE;
        }
        if (wType == (WORD)(UINT_PTR)RT_MENU)
        {
            // No output file
            return TRUE;
        }
        if (wType == (WORD)(UINT_PTR)RT_TOOLBAR)
        {
            // No output file
            return TRUE;
        }
        if (wType == (WORD)(UINT_PTR)RT_DIALOG)
        {
            // No output file
            return TRUE;
        }
        if (wType == (WORD)(UINT_PTR)RT_STRING)
        {
            // No output file
            return TRUE;
        }
        if (wType == (WORD)(UINT_PTR)RT_FONTDIR)
        {
            return TRUE;
        }
        if (wType == (WORD)(UINT_PTR)RT_FONT)
        {
            return g_res.extract_bin(filename.c_str(), entry);
        }
        if (wType == (WORD)(UINT_PTR)RT_ACCELERATOR)
        {
            // No output file
            return TRUE;
        }
        if (wType == (WORD)(UINT_PTR)RT_RCDATA)
        {
            return g_res.extract_bin(filename.c_str(), entry);
        }
        if (wType == (WORD)(UINT_PTR)RT_MESSAGETABLE)
        {
            return g_res.extract_bin(filename.c_str(), entry,pseudolocalize);

        }
        if (wType == (WORD)(UINT_PTR)RT_GROUP_CURSOR)
        {
            return g_res.extract_cursor(filename.c_str(), entry);
        }
        if (wType == (WORD)(UINT_PTR)RT_GROUP_ICON)
        {
            return g_res.extract_icon(filename.c_str(), entry);
        }
        if (wType == (WORD)(UINT_PTR)RT_VERSION)
        {
            // No output file
            return TRUE;
        }
        if (wType == (WORD)(UINT_PTR)RT_DLGINIT)
        {
            // No output file
            return TRUE;
        }
        if (wType == (WORD)(UINT_PTR)RT_DLGINCLUDE)
        {
            return g_res.extract_bin(filename.c_str(), entry);
        }
        if (wType == (WORD)(UINT_PTR)RT_PLUGPLAY)
        {
            return g_res.extract_bin(filename.c_str(), entry);
        }
        if (wType == (WORD)(UINT_PTR)RT_VXD)
        {
            return g_res.extract_bin(filename.c_str(), entry);
        }
        if (wType == (WORD)(UINT_PTR)RT_ANICURSOR)
        {
            return g_res.extract_cursor(filename.c_str(), entry);
        }
        if (wType == (WORD)(UINT_PTR)RT_ANIICON)
        {
            return g_res.extract_icon(filename.c_str(), entry);
        }
    }
    else
    {
        if (entry->m_type == L"AVI")
        {
            return g_res.extract_bin(filename.c_str(), entry);
        }
        if (entry->m_type == L"PNG")
        {
            return g_res.extract_bin(filename.c_str(), entry);
        }
        if (entry->m_type == L"GIF")
        {
            return g_res.extract_bin(filename.c_str(), entry);
        }
        if (entry->m_type == L"JPEG")
        {
            return g_res.extract_bin(filename.c_str(), entry);
        }
        if (entry->m_type == L"JPG")
        {
            return g_res.extract_bin(filename.c_str(), entry);
        }
        if (entry->m_type == L"TIFF")
        {
            return g_res.extract_bin(filename.c_str(), entry);
        }
        if (entry->m_type == L"TIF")
        {
            return g_res.extract_bin(filename.c_str(), entry);
        }
        if (entry->m_type == L"EMF")
        {
            return g_res.extract_bin(filename.c_str(), entry);
        }
        if (entry->m_type == L"ENHMETAFILE")
        {
            return g_res.extract_bin(filename.c_str(), entry);
        }
        if (entry->m_type == L"WMF")
        {
            return g_res.extract_bin(filename.c_str(), entry);
        }
        if (entry->m_type == L"WAVE")
        {
            return g_res.extract_bin(filename.c_str(), entry);
        }
        if (entry->m_type == L"IMAGE")
        {
            return g_res.extract_bin(filename.c_str(), entry);
        }
    }

    return g_res.extract_bin(filename.c_str(), entry,pseudolocalize);
}

// write the resource.h file
BOOL DoWriteRCLangUTF8(MFile& file, ResToText& res2text, WORD lang, const EntrySet& targets, bool pseudolocalize=false)
{


    // dump a comment and a LANGUAGE statement
    MString strLang = ::GetLanguageStatement(lang, TRUE,pseudolocalize);
    strLang += L"\r\n";
    file.WriteSzA(MWideToAnsi(CP_ACP, strLang.c_str()).c_str());

    // search the language entries
    EntrySet found;
    targets.search(found, ET_LANG, WORD(0), WORD(0), lang);

    std::vector<EntryBase*> vecFound(found.begin(), found.end());

    std::sort(vecFound.begin(), vecFound.end(),
        [](const EntryBase* a, const EntryBase* b) {
            if (a->m_type < b->m_type)
            return true;
    if (a->m_type > b->m_type)
        return false;
    return a->m_name < b->m_name;
        }
    );

    MIdOrString type, old_type;

    // for all found entries
    for (auto entry : vecFound)
    {
        old_type = type;
        type = entry->m_type;

        // ignore the string or message tables or font dir
        if (type == RT_STRING || type == RT_FONTDIR)
            continue;

        // dump the entry
        MString str = res2text.DumpEntry(*entry,pseudolocalize);
        if (!str.empty())
        {
            // output redundant comments
            if (type != old_type && g_settings.bRedundantComments)
            {
                MStringW strType = res2text.GetResTypeName(type);
                MWideToAnsi utf8(CP_UTF8, strType.c_str());
                file.WriteSzA("// ");
                file.WriteSzA(utf8.c_str());
                file.WriteSzA("\r\n\r\n");
            }

            mstr_trim(str);     // trim

            // convert the text to UTF-8
            MTextToAnsi t2a(CP_UTF8, str.c_str());
            file.WriteSzA(t2a.c_str());

            // add newlines
            file.WriteSzA("\r\n\r\n");
        }
    }

    // search the string tables
    found.clear();
    targets.search(found, ET_LANG, RT_STRING, (WORD)0, lang);
    if (found.size())
    {
        if (g_settings.bRedundantComments)
        {
            file.WriteSzA("// RT_STRING\r\n\r\n");
        }

        // found --> str_res
        StringRes str_res;
        for (auto e : found)
        {
            if (e->m_lang != lang)
                continue;       // must be same language

            MByteStreamEx stream(e->m_data);
            if (!str_res.LoadFromStream(stream, e->m_name.m_id))
                return FALSE;
        }

        // dump
        MString str = str_res.Dump(pseudolocalize);

        // trim
        mstr_trim(str);

        // append newlines
        str += L"\r\n\r\n";

        // convert the text to UTF-8
        MTextToAnsi t2a(CP_UTF8, str.c_str());

        // write it
        file.WriteSzA(t2a.c_str());
    }

    // search the message tables
    found.clear();

    return TRUE;
}

BOOL DoWriteRC(LPCWSTR pszFileName, LPCWSTR pszResH, const EntrySet& found, bool pseudolocalize=false)
{
    ResToText res2text;
    res2text.m_bHumanReadable = FALSE;  // it's not human-friendly
    res2text.m_bNoLanguage = TRUE;      // no LANGUAGE statements generated



    // create a RC file
    MFile file(pszFileName, TRUE);
    if (!file)
        return FALSE;

    BOOL bRCFileUTF16 = false;

    WCHAR szTitle[MAX_PATH];
    GetFileTitleW(pszFileName, szTitle, _countof(szTitle));

    // dump heading
    if (bRCFileUTF16)
    {
        file.WriteFormatW(L"// %s\r\n", szTitle);

        file.WriteSzW(L"\r\n");

        if (pszResH && pszResH[0])
            file.WriteSzW(L"#include \"resource.h\"\r\n");
        file.WriteSzW(L"#define APSTUDIO_HIDDEN_SYMBOLS\r\n");
        file.WriteSzW(L"#include <windows.h>\r\n");
        file.WriteSzW(L"#include <commctrl.h>\r\n");
        file.WriteSzW(L"#undef APSTUDIO_HIDDEN_SYMBOLS\r\n");
        file.WriteSzW(L"#pragma code_page(65001) // UTF-8\r\n\r\n");

        if (false && !false)
        {
            file.WriteSzW(L"#ifndef IDC_STATIC\r\n");
            file.WriteSzW(L"    #define IDC_STATIC (-1)\r\n");
            file.WriteSzW(L"#endif\r\n\r\n");
        }
    }
    else
    {
        MWideToAnsi utf8(CP_UTF8, szTitle);
        file.WriteFormatA("// %s\r\n", utf8.c_str());

        file.WriteSzA("\r\n");

        if (pszResH && pszResH[0])
            file.WriteSzA("#include \"resource.h\"\r\n");
        file.WriteSzA("#define APSTUDIO_HIDDEN_SYMBOLS\r\n");
        file.WriteSzA("#include <windows.h>\r\n");
        file.WriteSzA("#include <commctrl.h>\r\n");
        file.WriteSzA("#undef APSTUDIO_HIDDEN_SYMBOLS\r\n");
        file.WriteSzA("#pragma code_page(65001) // UTF-8\r\n\r\n");

        if (false && !false)
        {
            file.WriteSzA("#ifndef IDC_STATIC\r\n");
            file.WriteSzA("    #define IDC_STATIC (-1)\r\n");
            file.WriteSzA("#endif\r\n\r\n");
        }
    }

    // get the used languages
    std::unordered_set<WORD> langs;
    typedef std::pair<WORD, MStringW> lang_pair;
    std::vector<lang_pair> lang_vec;

    for (auto res : found)
    {
        WORD lang = res->m_lang;
        if (langs.insert(lang).second)
        {
            MString lang_name = g_db.GetLangName(lang);
            lang_vec.push_back(std::make_pair(lang, lang_name));
        }
    }

    // sort by lang_name
    std::sort(lang_vec.begin(), lang_vec.end(),
        [](const lang_pair& a, const lang_pair& b) {
            return (a.second < b.second);
        }
    );

    // add "res/" to the prefix if necessary
    if (g_settings.bStoreToResFolder)
        res2text.m_strFilePrefix = L"res/";

    // use the "lang" folder?
    if (g_settings.bSepFilesByLang)
    {
        // dump neutral
        if (langs.count(0) > 0)
        {
            if (!DoWriteRCLangUTF8(file, res2text, 0, found,pseudolocalize))
                return FALSE;
        }

        // create "lang" directory path
        WCHAR szLangDir[MAX_PATH];
        StringCchCopyW(szLangDir, _countof(szLangDir), pszFileName);

        // find the last '\\' or '/'
        WCHAR* pch = wcsrchr(szLangDir, L'\\');
        if (pch == NULL)
            pch = mstrrchr(szLangDir, L'/');
        if (pch == NULL)
            return FALSE;

        // build the lang directory path
        *pch = 0;
        StringCchCatW(szLangDir, _countof(szLangDir), TEXT("/lang"));

        // backup and create "lang" directory
        for (auto lang_pair : lang_vec)
        {
            if (!lang_pair.first)
                continue;

            CreateDirectory(szLangDir, NULL);
            break;
        }

        // for each language
        for (auto lang_pair : lang_vec)
        {
            auto lang = lang_pair.first;
            if (!lang)
                continue;

            // create lang/XX_XX.rc file
            WCHAR szLangFile[MAX_PATH];
            StringCchCopyW(szLangFile, _countof(szLangFile), szLangDir);
            StringCchCatW(szLangFile, _countof(szLangFile), TEXT("/"));
            MStringW lang_name = lang_pair.second;
            StringCchCatW(szLangFile, _countof(szLangFile), lang_name.c_str());
            StringCchCatW(szLangFile, _countof(szLangFile), TEXT(".rc"));
            //MessageBox(NULL, szLangFile, NULL, 0);

            // dump to lang/XX_XX.rc file
            MFile lang_file(szLangFile, TRUE);
            if (bRCFileUTF16)
            {
                lang_file.WriteSzW(L"\r\n");
                lang_file.WriteSzW(L"#pragma code_page(65001) // UTF-8\r\n\r\n");
            }
            else
            {
                lang_file.WriteSzA("\r\n");
                lang_file.WriteSzA("#pragma code_page(65001) // UTF-8\r\n\r\n");
            }
            if (!lang_file)
                return FALSE;
            if (!DoWriteRCLangUTF8(lang_file, res2text, lang, found,pseudolocalize))
                return FALSE;

            if (g_settings.bRedundantComments)
            {
            }
        }
    }
    else
    {
        // don't use the "lang" folder
        for (auto lang_pair : lang_vec)
        {
            auto lang = lang_pair.first;
            // write it for each language
            if (!DoWriteRCLangUTF8(file, res2text, lang, found,pseudolocalize))
                return FALSE;
        }
    }

    // dump language includes
    if (g_settings.bSepFilesByLang)
    {
        // write a C++ comment to make a section
        if (g_settings.bRedundantComments)
        {
            if (bRCFileUTF16)
            {
                file.WriteSzW(L"// Languages\r\n\r\n");
            }
            else
            {
                file.WriteSzA("// Languages\r\n\r\n");
            }
        }

        if (g_settings.bSelectableByMacro)
        {
            for (auto lang_pair : lang_vec)     // for each language
            {
                auto lang = lang_pair.first;
                if (!lang)
                    continue;       // ignore neutral language

                // get the language name (such as en_US, ja_JP, etc.) from database
                MString lang_name1 = g_db.GetLangName(lang);

                // make uppercase one
                MString lang_name2 = lang_name1;
                CharUpperW(&lang_name2[0]);

                if (bRCFileUTF16)
                {
                    // write "#ifdef LANGUAGE_...\r\n"
                    file.WriteSzW(L"#ifdef LANGUAGE_");
                    file.WriteSzW(lang_name2.c_str());
                    file.WriteSzW(L"\r\n");

                    // write "#define \"lang/....rc\"\r\n"
                    file.WriteSzW(L"    #include \"lang/");
                    file.WriteSzW(lang_name1.c_str());
                    file.WriteSzW(L".rc\"\r\n");

                    // write "#endif\r\n"
                    file.WriteSzW(L"#endif\r\n");
                }
                else
                {
                    // write "#ifdef LANGUAGE_...\r\n"
                    file.WriteSzA("#ifdef LANGUAGE_");
                    MWideToAnsi lang2_w2a(CP_ACP, lang_name2.c_str());
                    file.WriteSzA(lang2_w2a.c_str());
                    file.WriteSzA("\r\n");

                    // write "#define \"lang/....rc\"\r\n"
                    file.WriteSzA("    #include \"lang/");
                    MWideToAnsi lang1_w2a(CP_ACP, lang_name1.c_str());
                    file.WriteSzA(lang1_w2a.c_str());
                    file.WriteSzA(".rc\"\r\n");

                    // write "#endif\r\n"
                    file.WriteSzA("#endif\r\n");
                }
            }
        }
        else
        {
            for (auto lang_pair : lang_vec)
            {
                auto lang = lang_pair.first;
                if (!lang)
                    continue;   // ignore the neutral language

                // get the language name (such as en_US, ja_JP, etc.) from database
                MString lang_name1 = g_db.GetLangName(lang);

                if (bRCFileUTF16)
                {
                    // write "#include \"lang/....rc\"\r\n"
                    file.WriteSzW(L"#include \"lang/");
                    file.WriteSzW(lang_name1.c_str());
                    file.WriteSzW(L".rc\"\r\n");
                }
                else
                {
                    // write "#include \"lang/....rc\"\r\n"
                    file.WriteSzA("#include \"lang/");
                    file.WriteSzA(MWideToAnsi(CP_ACP, lang_name1.c_str()).c_str());
                    file.WriteSzA(".rc\"\r\n");
                }
            }
        }

        if (bRCFileUTF16)
            file.WriteSzW(L"\r\n");
        else
            file.WriteSzA("\r\n");
    }

    if (g_settings.bRedundantComments)
    {
        if (bRCFileUTF16)
        {
            file.WriteSzW(L"// TEXTINCLUDE\r\n\r\n");
        }
        else
        {
            file.WriteSzA("// TEXTINCLUDE\r\n\r\n");
        }
    }

    if (bRCFileUTF16)
    {
        file.WriteSzW(L"#ifdef APSTUDIO_INVOKED\r\n\r\n");

        // write three TEXTINCLUDE's
        file.WriteSzW(L"1 TEXTINCLUDE\r\n");
        file.WriteSzW(L"BEGIN\r\n");
        file.WriteSzW(L"    \"resource.h\\0\"\r\n");
        file.WriteSzW(L"END\r\n\r\n");

        file.WriteSzW(L"2 TEXTINCLUDE\r\n");
        file.WriteSzW(L"BEGIN\r\n");
        file.WriteSzW(L"    \"#define APSTUDIO_HIDDEN_SYMBOLS\\r\\n\"\r\n");
        file.WriteSzW(L"    \"#include <windows.h>\\r\\n\"\r\n");
        file.WriteSzW(L"    \"#include <commctrl.h>\\r\\n\"\r\n");
        file.WriteSzW(L"    \"#undef APSTUDIO_HIDDEN_SYMBOLS\\r\\n\"\r\n");
        file.WriteSzW(L"    \"\\0\"\r\n");
        file.WriteSzW(L"END\r\n\r\n");

        file.WriteSzW(L"3 TEXTINCLUDE\r\n");
        file.WriteSzW(L"BEGIN\r\n");
        file.WriteSzW(L"    \"\\r\\n\"\r\n");
        file.WriteSzW(L"    \"\\0\"\r\n");
        file.WriteSzW(L"END\r\n\r\n");

        file.WriteSzW(L"#endif    // APSTUDIO_INVOKED\r\n");

        if (g_settings.bRedundantComments)
        {
            file.WriteSzW(L"\r\n");
        }
    }
    else
    {
        file.WriteSzA("#ifdef APSTUDIO_INVOKED\r\n\r\n");

        // write three TEXTINCLUDE's
        file.WriteSzA("1 TEXTINCLUDE\r\n");
        file.WriteSzA("BEGIN\r\n");
        file.WriteSzA("    \"resource.h\\0\"\r\n");
        file.WriteSzA("END\r\n\r\n");

        file.WriteSzA("2 TEXTINCLUDE\r\n");
        file.WriteSzA("BEGIN\r\n");
        file.WriteSzA("    \"#define APSTUDIO_HIDDEN_SYMBOLS\\r\\n\"\r\n");
        file.WriteSzA("    \"#include <windows.h>\\r\\n\"\r\n");
        file.WriteSzA("    \"#include <commctrl.h>\\r\\n\"\r\n");
        file.WriteSzA("    \"#undef APSTUDIO_HIDDEN_SYMBOLS\\r\\n\"\r\n");
        file.WriteSzA("    \"\\0\"\r\n");
        file.WriteSzA("END\r\n\r\n");

        file.WriteSzA("3 TEXTINCLUDE\r\n");
        file.WriteSzA("BEGIN\r\n");
        file.WriteSzA("    \"\\r\\n\"\r\n");
        file.WriteSzA("    \"\\0\"\r\n");
        file.WriteSzA("END\r\n\r\n");

        file.WriteSzA("#endif    // APSTUDIO_INVOKED\r\n");

        if (g_settings.bRedundantComments)
        {
            file.WriteSzA("\r\n");
        }
    }

    return TRUE;
}
BOOL DoExport(LPCWSTR pszRCFile, LPWSTR pszResHFile, const EntrySet& found , bool pseudolocalize=false)
{
    if (found.empty())
    {
        // unable to export the empty data
        return FALSE;
    }

    // pszRCFile --> szPath
    WCHAR szPath[MAX_PATH];
    StringCchCopy(szPath, _countof(szPath), pszRCFile);

    // find the '\\' or '/' character
    WCHAR* pch = mstrrchr(szPath, L'\\');
    if (!pch)
        pch = mstrrchr(szPath, L'/');
    if (!pch)
        return FALSE;   // failure

    *pch = 0;

    // check whether there is an external file to be extracted
    BOOL bHasExternFile = FALSE;
    for (auto e : found)
    {
        ResToText res2text;
        MString filename = res2text.GetEntryFileName(*e);
        if (filename.size())
        {
            bHasExternFile = TRUE;
            break;
        }
    }

    // save the current directory and move the current directory
    WCHAR szCurDir[MAX_PATH];
    GetCurrentDirectory(_countof(szCurDir), szCurDir);
    if (!SetCurrentDirectory(szPath))
        return FALSE;

    if (bHasExternFile)
    {
        // create the "res" folder (with backuping) if necessary
        if (g_settings.bStoreToResFolder)
        {
            MString strResDir = szPath;
            strResDir += TEXT("\\res");

            CreateDirectory(strResDir.c_str(), NULL);
        }

        // extract each data if necessary
        for (auto e : found)
        {
            if (e->m_type == RT_STRING || e->m_type == RT_FONTDIR)
                continue;
            if (!DoExtract(e, TRUE,pseudolocalize))
                return FALSE;
        }
    }

    BOOL bOK = FALSE;
    if ((m_szResourceH[0] || !g_settings.IsIDMapEmpty()) &&
        !false)
    {
        // build the resource.h file path
        *pch = 0;
        StringCchCatW(szPath, _countof(szPath), L"\\resource.h");

        // write the resource.h file and the RC file
        bOK = DoWriteRC(pszRCFile, szPath, found,pseudolocalize);

        // szPath --> pszResHFile
        if (bOK && pszResHFile)
            StringCchCopyW(pszResHFile, MAX_PATH, szPath);
    }
    else
    {
        // write the RC file
        bOK = DoWriteRC(pszRCFile, NULL, found,pseudolocalize);
    }

    // resume the current directory
    SetCurrentDirectory(szCurDir);


    return bOK;
}
BOOL DoExport(LPCWSTR pszRCFile, LPWSTR pszResHFile, bool pseudolocalize=false)
{
    // search the language entries
    EntrySet found;
    g_res.search(found, ET_LANG);

    return DoExport(pszRCFile, pszResHFile, found,pseudolocalize);
}
MString GetLanguageStatement(WORD langid, BOOL bOldStyle, bool pseudolocalize = false)
{
    if (pseudolocalize) return L"LANGUAGE 0x101,0x1\r\n";
    MString strPrim, strSub;

#define SWITCH_SUBLANG() switch (SUBLANGID(langid))

    // try to get the primary language name and the sub-language name
    switch (PRIMARYLANGID(langid))
    {
    case LANG_NEUTRAL: strPrim = TEXT("LANG_NEUTRAL");
        break;
    case LANG_INVARIANT: strPrim = TEXT("LANG_INVARIANT");
        break;
    case LANG_AFRIKAANS: strPrim = TEXT("LANG_AFRIKAANS");
        SWITCH_SUBLANG()
        {
    case SUBLANG_AFRIKAANS_SOUTH_AFRICA: strSub = TEXT("SUBLANG_AFRIKAANS_SOUTH_AFRICA"); break;
        }
        break;
    case LANG_ALBANIAN: strPrim = TEXT("LANG_ALBANIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_ALBANIAN_ALBANIA: strSub = TEXT("SUBLANG_ALBANIAN_ALBANIA"); break;
        }
        break;
    case LANG_ALSATIAN: strPrim = TEXT("LANG_ALSATIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_ALSATIAN_FRANCE: strSub = TEXT("SUBLANG_ALSATIAN_FRANCE"); break;
        }
        break;
    case LANG_AMHARIC: strPrim = TEXT("LANG_AMHARIC");
        SWITCH_SUBLANG()
        {
    case SUBLANG_AMHARIC_ETHIOPIA: strSub = TEXT("SUBLANG_AMHARIC_ETHIOPIA"); break;
        }
        break;
    case LANG_ARABIC: strPrim = TEXT("LANG_ARABIC");
        SWITCH_SUBLANG()
        {
    case SUBLANG_ARABIC_SAUDI_ARABIA: strSub = TEXT("SUBLANG_ARABIC_SAUDI_ARABIA"); break;
    case SUBLANG_ARABIC_IRAQ: strSub = TEXT("SUBLANG_ARABIC_IRAQ"); break;
    case SUBLANG_ARABIC_EGYPT: strSub = TEXT("SUBLANG_ARABIC_EGYPT"); break;
    case SUBLANG_ARABIC_LIBYA: strSub = TEXT("SUBLANG_ARABIC_LIBYA"); break;
    case SUBLANG_ARABIC_ALGERIA: strSub = TEXT("SUBLANG_ARABIC_ALGERIA"); break;
    case SUBLANG_ARABIC_MOROCCO: strSub = TEXT("SUBLANG_ARABIC_MOROCCO"); break;
    case SUBLANG_ARABIC_TUNISIA: strSub = TEXT("SUBLANG_ARABIC_TUNISIA"); break;
    case SUBLANG_ARABIC_OMAN: strSub = TEXT("SUBLANG_ARABIC_OMAN"); break;
    case SUBLANG_ARABIC_YEMEN: strSub = TEXT("SUBLANG_ARABIC_YEMEN"); break;
    case SUBLANG_ARABIC_SYRIA: strSub = TEXT("SUBLANG_ARABIC_SYRIA"); break;
    case SUBLANG_ARABIC_JORDAN: strSub = TEXT("SUBLANG_ARABIC_JORDAN"); break;
    case SUBLANG_ARABIC_LEBANON: strSub = TEXT("SUBLANG_ARABIC_LEBANON"); break;
    case SUBLANG_ARABIC_KUWAIT: strSub = TEXT("SUBLANG_ARABIC_KUWAIT"); break;
    case SUBLANG_ARABIC_UAE: strSub = TEXT("SUBLANG_ARABIC_UAE"); break;
    case SUBLANG_ARABIC_BAHRAIN: strSub = TEXT("SUBLANG_ARABIC_BAHRAIN"); break;
    case SUBLANG_ARABIC_QATAR: strSub = TEXT("SUBLANG_ARABIC_QATAR"); break;
        }
        break;
    case LANG_ARMENIAN: strPrim = TEXT("LANG_ARMENIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_ARMENIAN_ARMENIA: strSub = TEXT("SUBLANG_ARMENIAN_ARMENIA"); break;
        }
        break;
    case LANG_ASSAMESE: strPrim = TEXT("LANG_ASSAMESE");
        SWITCH_SUBLANG()
        {
    case SUBLANG_ASSAMESE_INDIA: strSub = TEXT("SUBLANG_ASSAMESE_INDIA"); break;
        }
        break;
    case LANG_AZERI: strPrim = TEXT("LANG_AZERI");
        SWITCH_SUBLANG()
        {
    case SUBLANG_AZERI_LATIN: strSub = TEXT("SUBLANG_AZERI_LATIN"); break;
    case SUBLANG_AZERI_CYRILLIC: strSub = TEXT("SUBLANG_AZERI_CYRILLIC"); break;
        }
        break;
        //case LANG_AZERBAIJANI: strPrim = TEXT("LANG_AZERBAIJANI"); // same as LANG_AZERI
        //    SWITCH_SUBLANG()
        //    {
        //        case SUBLANG_AZERBAIJANI_AZERBAIJAN_LATIN: strSub = TEXT("SUBLANG_AZERBAIJANI_AZERBAIJAN_LATIN"); break;
        //        case SUBLANG_AZERBAIJANI_AZERBAIJAN_CYRILLIC: strSub = TEXT("SUBLANG_AZERBAIJANI_AZERBAIJAN_CYRILLIC"); break;
        //    }
        //    break;
        //case LANG_BANGLA: strPrim = TEXT("LANG_BANGLA"); // same as LANG_BENGALI
        //    SWITCH_SUBLANG()
        //    {
        //    case SUBLANG_BANGLA_INDIA: strSub = TEXT("SUBLANG_BANGLA_INDIA"); break;
        //    case SUBLANG_BANGLA_BANGLADESH: strSub = TEXT("SUBLANG_BANGLA_BANGLADESH"); break;
        //    }
        //    break;
    case LANG_BASHKIR: strPrim = TEXT("LANG_BASHKIR");
        SWITCH_SUBLANG()
        {
    case SUBLANG_BASHKIR_RUSSIA: strSub = TEXT("SUBLANG_BASHKIR_RUSSIA"); break;
        }
        break;
    case LANG_BASQUE: strPrim = TEXT("LANG_BASQUE");
        SWITCH_SUBLANG()
        {
    case SUBLANG_BASQUE_BASQUE: strSub = TEXT("SUBLANG_BASQUE_BASQUE"); break;
        }
        break;
    case LANG_BELARUSIAN: strPrim = TEXT("LANG_BELARUSIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_BELARUSIAN_BELARUS: strSub = TEXT("SUBLANG_BELARUSIAN_BELARUS"); break;
        }
        break;
    case LANG_BENGALI: strPrim = TEXT("LANG_BENGALI");
        SWITCH_SUBLANG()
        {
    case SUBLANG_BENGALI_INDIA: strSub = TEXT("SUBLANG_BENGALI_INDIA"); break;
    case SUBLANG_BENGALI_BANGLADESH: strSub = TEXT("SUBLANG_BENGALI_BANGLADESH"); break;
        }
        break;
    case LANG_BRETON: strPrim = TEXT("LANG_BRETON");
        SWITCH_SUBLANG()
        {
    case SUBLANG_BRETON_FRANCE: strSub = TEXT("SUBLANG_BRETON_FRANCE"); break;
        }
        break;
    case LANG_BOSNIAN: strPrim = TEXT("LANG_BOSNIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_BOSNIAN_BOSNIA_HERZEGOVINA_LATIN: strSub = TEXT("SUBLANG_BOSNIAN_BOSNIA_HERZEGOVINA_LATIN"); break;
    case SUBLANG_BOSNIAN_BOSNIA_HERZEGOVINA_CYRILLIC: strSub = TEXT("SUBLANG_BOSNIAN_BOSNIA_HERZEGOVINA_CYRILLIC"); break;
        }
        break;
    case LANG_BOSNIAN_NEUTRAL: strPrim = TEXT("LANG_BOSNIAN_NEUTRAL");
        SWITCH_SUBLANG()
        {
    case SUBLANG_BOSNIAN_BOSNIA_HERZEGOVINA_LATIN: strSub = TEXT("SUBLANG_BOSNIAN_BOSNIA_HERZEGOVINA_LATIN"); break;
    case SUBLANG_BOSNIAN_BOSNIA_HERZEGOVINA_CYRILLIC: strSub = TEXT("SUBLANG_BOSNIAN_BOSNIA_HERZEGOVINA_CYRILLIC"); break;
        }
        break;
    case LANG_BULGARIAN: strPrim = TEXT("LANG_BULGARIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_BULGARIAN_BULGARIA: strSub = TEXT("SUBLANG_BULGARIAN_BULGARIA"); break;
        }
        break;
    case LANG_CATALAN: strPrim = TEXT("LANG_CATALAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_CATALAN_CATALAN: strSub = TEXT("SUBLANG_CATALAN_CATALAN"); break;
        }
        break;
#ifdef ENABLE_NEW_LANGS
    case LANG_CENTRAL_KURDISH: strPrim = TEXT("LANG_CENTRAL_KURDISH");
        SWITCH_SUBLANG()
        {
    case SUBLANG_CENTRAL_KURDISH_IRAQ: strSub = TEXT("SUBLANG_CENTRAL_KURDISH_IRAQ"); break;
        }
        break;
#endif
#ifdef ENABLE_NEW_LANGS
    case LANG_CHEROKEE: strPrim = TEXT("LANG_CHEROKEE");
        SWITCH_SUBLANG()
        {
    case SUBLANG_CHEROKEE_CHEROKEE: strSub = TEXT("SUBLANG_CHEROKEE_CHEROKEE"); break;
        }
        break;
#endif
    case LANG_CHINESE: strPrim = TEXT("LANG_CHINESE");
        SWITCH_SUBLANG()
        {
    case SUBLANG_CHINESE_TRADITIONAL: strSub = TEXT("SUBLANG_CHINESE_TRADITIONAL"); break;
    case SUBLANG_CHINESE_SIMPLIFIED: strSub = TEXT("SUBLANG_CHINESE_SIMPLIFIED"); break;
    case SUBLANG_CHINESE_HONGKONG: strSub = TEXT("SUBLANG_CHINESE_HONGKONG"); break;
    case SUBLANG_CHINESE_SINGAPORE: strSub = TEXT("SUBLANG_CHINESE_SINGAPORE"); break;
    case SUBLANG_CHINESE_MACAU: strSub = TEXT("SUBLANG_CHINESE_MACAU"); break;
        }
        break;
        //case LANG_CHINESE_SIMPLIFIED: strPrim = TEXT("LANG_CHINESE_SIMPLIFIED"); // same as LANG_CHINESE
        //    SWITCH_SUBLANG()
        //    {
        //        case SUBLANG_CHINESE_TRADITIONAL: strSub = TEXT("SUBLANG_CHINESE_TRADITIONAL"); break;
        //        case SUBLANG_CHINESE_SIMPLIFIED: strSub = TEXT("SUBLANG_CHINESE_SIMPLIFIED"); break;
        //        case SUBLANG_CHINESE_HONGKONG: strSub = TEXT("SUBLANG_CHINESE_HONGKONG"); break;
        //        case SUBLANG_CHINESE_SINGAPORE: strSub = TEXT("SUBLANG_CHINESE_SINGAPORE"); break;
        //        case SUBLANG_CHINESE_MACAU: strSub = TEXT("SUBLANG_CHINESE_MACAU"); break;
        //    }
        //    break;
    case LANG_CHINESE_TRADITIONAL: strPrim = TEXT("LANG_CHINESE_TRADITIONAL");
        SWITCH_SUBLANG()
        {
    case SUBLANG_CHINESE_TRADITIONAL: strSub = TEXT("SUBLANG_CHINESE_TRADITIONAL"); break;
    case SUBLANG_CHINESE_SIMPLIFIED: strSub = TEXT("SUBLANG_CHINESE_SIMPLIFIED"); break;
    case SUBLANG_CHINESE_HONGKONG: strSub = TEXT("SUBLANG_CHINESE_HONGKONG"); break;
    case SUBLANG_CHINESE_SINGAPORE: strSub = TEXT("SUBLANG_CHINESE_SINGAPORE"); break;
    case SUBLANG_CHINESE_MACAU: strSub = TEXT("SUBLANG_CHINESE_MACAU"); break;
        }
        break;
    case LANG_CORSICAN: strPrim = TEXT("LANG_CORSICAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_CORSICAN_FRANCE: strSub = TEXT("SUBLANG_CORSICAN_FRANCE"); break;
        }
        break;
        //case LANG_CROATIAN: strPrim = TEXT("LANG_CROATIAN"); // same as LANG_BOSNIAN
        //    SWITCH_SUBLANG()
        //    {
        //        case SUBLANG_CROATIAN_CROATIA: strSub = TEXT("SUBLANG_CROATIAN_CROATIA"); break;
        //        case SUBLANG_CROATIAN_BOSNIA_HERZEGOVINA_LATIN: strSub = TEXT("SUBLANG_CROATIAN_BOSNIA_HERZEGOVINA_LATIN"); break;
        //    }
        //    break;
    case LANG_CZECH: strPrim = TEXT("LANG_CZECH");
        SWITCH_SUBLANG()
        {
    case SUBLANG_CZECH_CZECH_REPUBLIC: strSub = TEXT("SUBLANG_CZECH_CZECH_REPUBLIC"); break;
        }
        break;
    case LANG_DANISH: strPrim = TEXT("LANG_DANISH");
        SWITCH_SUBLANG()
        {
    case SUBLANG_DANISH_DENMARK: strSub = TEXT("SUBLANG_DANISH_DENMARK"); break;
        }
        break;
    case LANG_DARI: strPrim = TEXT("LANG_DARI");
        SWITCH_SUBLANG()
        {
    case SUBLANG_DARI_AFGHANISTAN: strSub = TEXT("SUBLANG_DARI_AFGHANISTAN"); break;
        }
        break;
    case LANG_DIVEHI: strPrim = TEXT("LANG_DIVEHI");
        SWITCH_SUBLANG()
        {
    case SUBLANG_DIVEHI_MALDIVES: strSub = TEXT("SUBLANG_DIVEHI_MALDIVES"); break;
        }
        break;
    case LANG_DUTCH: strPrim = TEXT("LANG_DUTCH");
        SWITCH_SUBLANG()
        {
    case SUBLANG_DUTCH: strSub = TEXT("SUBLANG_DUTCH"); break;
    case SUBLANG_DUTCH_BELGIAN: strSub = TEXT("SUBLANG_DUTCH_BELGIAN"); break;
        }
        break;
    case LANG_ENGLISH: strPrim = TEXT("LANG_ENGLISH");
        SWITCH_SUBLANG()
        {
    case SUBLANG_ENGLISH_US: strSub = TEXT("SUBLANG_ENGLISH_US"); break;
    case SUBLANG_ENGLISH_UK: strSub = TEXT("SUBLANG_ENGLISH_UK"); break;
    case SUBLANG_ENGLISH_AUS: strSub = TEXT("SUBLANG_ENGLISH_AUS"); break;
    case SUBLANG_ENGLISH_CAN: strSub = TEXT("SUBLANG_ENGLISH_CAN"); break;
    case SUBLANG_ENGLISH_NZ: strSub = TEXT("SUBLANG_ENGLISH_NZ"); break;
        //case SUBLANG_ENGLISH_IRELAND: strSub = TEXT("SUBLANG_ENGLISH_IRELAND"); break; // same as SUBLANG_ENGLISH_EIRE
    case SUBLANG_ENGLISH_EIRE: strSub = TEXT("SUBLANG_ENGLISH_EIRE"); break;
    case SUBLANG_ENGLISH_SOUTH_AFRICA: strSub = TEXT("SUBLANG_ENGLISH_SOUTH_AFRICA"); break;
    case SUBLANG_ENGLISH_JAMAICA: strSub = TEXT("SUBLANG_ENGLISH_JAMAICA"); break;
    case SUBLANG_ENGLISH_CARIBBEAN: strSub = TEXT("SUBLANG_ENGLISH_CARIBBEAN"); break;
    case SUBLANG_ENGLISH_BELIZE: strSub = TEXT("SUBLANG_ENGLISH_BELIZE"); break;
    case SUBLANG_ENGLISH_TRINIDAD: strSub = TEXT("SUBLANG_ENGLISH_TRINIDAD"); break;
    case SUBLANG_ENGLISH_ZIMBABWE: strSub = TEXT("SUBLANG_ENGLISH_ZIMBABWE"); break;
    case SUBLANG_ENGLISH_PHILIPPINES: strSub = TEXT("SUBLANG_ENGLISH_PHILIPPINES"); break;
    case SUBLANG_ENGLISH_INDIA: strSub = TEXT("SUBLANG_ENGLISH_INDIA"); break;
    case SUBLANG_ENGLISH_MALAYSIA: strSub = TEXT("SUBLANG_ENGLISH_MALAYSIA"); break;
    case SUBLANG_ENGLISH_SINGAPORE: strSub = TEXT("SUBLANG_ENGLISH_SINGAPORE"); break;
        }
        break;
    case LANG_ESTONIAN: strPrim = TEXT("LANG_ESTONIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_ESTONIAN_ESTONIA: strSub = TEXT("SUBLANG_ESTONIAN_ESTONIA"); break;
        }
        break;
    case LANG_FAEROESE: strPrim = TEXT("LANG_FAEROESE");
        SWITCH_SUBLANG()
        {
    case SUBLANG_FAEROESE_FAROE_ISLANDS: strSub = TEXT("SUBLANG_FAEROESE_FAROE_ISLANDS"); break;
        }
        break;
        //case LANG_FARSI: strPrim = TEXT("LANG_FARSI"); // same as LANG_PERSIAN
        //    break;
    case LANG_FILIPINO: strPrim = TEXT("LANG_FILIPINO");
        SWITCH_SUBLANG()
        {
    case SUBLANG_FILIPINO_PHILIPPINES: strSub = TEXT("SUBLANG_FILIPINO_PHILIPPINES"); break;
        }
        break;
    case LANG_FINNISH: strPrim = TEXT("LANG_FINNISH");
        SWITCH_SUBLANG()
        {
    case SUBLANG_FINNISH_FINLAND: strSub = TEXT("SUBLANG_FINNISH_FINLAND"); break;
        }
        break;
    case LANG_FRENCH: strPrim = TEXT("LANG_FRENCH");
        SWITCH_SUBLANG()
        {
    case SUBLANG_FRENCH: strSub = TEXT("SUBLANG_FRENCH"); break;
    case SUBLANG_FRENCH_BELGIAN: strSub = TEXT("SUBLANG_FRENCH_BELGIAN"); break;
    case SUBLANG_FRENCH_CANADIAN: strSub = TEXT("SUBLANG_FRENCH_CANADIAN"); break;
    case SUBLANG_FRENCH_SWISS: strSub = TEXT("SUBLANG_FRENCH_SWISS"); break;
    case SUBLANG_FRENCH_LUXEMBOURG: strSub = TEXT("SUBLANG_FRENCH_LUXEMBOURG"); break;
    case SUBLANG_FRENCH_MONACO: strSub = TEXT("SUBLANG_FRENCH_MONACO"); break;
        }
        break;
    case LANG_FRISIAN: strPrim = TEXT("LANG_FRISIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_FRISIAN_NETHERLANDS: strSub = TEXT("SUBLANG_FRISIAN_NETHERLANDS"); break;
        }
        break;
        //case LANG_FULAH: strPrim = TEXT("LANG_FULAH"); // same as LANG_PULAR
        //    SWITCH_SUBLANG()
        //    {
        //    case SUBLANG_FULAH_SENEGAL: strSub = TEXT("SUBLANG_FULAH_SENEGAL"); break;
        //    }
        //    break;
    case LANG_GALICIAN: strPrim = TEXT("LANG_GALICIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_GALICIAN_GALICIAN: strSub = TEXT("SUBLANG_GALICIAN_GALICIAN"); break;
        }
        break;
    case LANG_GEORGIAN: strPrim = TEXT("LANG_GEORGIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_GEORGIAN_GEORGIA: strSub = TEXT("SUBLANG_GEORGIAN_GEORGIA"); break;
        }
        break;
    case LANG_GERMAN: strPrim = TEXT("LANG_GERMAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_GERMAN: strSub = TEXT("SUBLANG_GERMAN"); break;
    case SUBLANG_GERMAN_SWISS: strSub = TEXT("SUBLANG_GERMAN_SWISS"); break;
    case SUBLANG_GERMAN_AUSTRIAN: strSub = TEXT("SUBLANG_GERMAN_AUSTRIAN"); break;
    case SUBLANG_GERMAN_LUXEMBOURG: strSub = TEXT("SUBLANG_GERMAN_LUXEMBOURG"); break;
    case SUBLANG_GERMAN_LIECHTENSTEIN: strSub = TEXT("SUBLANG_GERMAN_LIECHTENSTEIN"); break;
        }
        break;
    case LANG_GREEK: strPrim = TEXT("LANG_GREEK");
        SWITCH_SUBLANG()
        {
    case SUBLANG_GREEK_GREECE: strSub = TEXT("SUBLANG_GREEK_GREECE"); break;
        }
        break;
    case LANG_GREENLANDIC: strPrim = TEXT("LANG_GREENLANDIC");
        SWITCH_SUBLANG()
        {
    case SUBLANG_GREENLANDIC_GREENLAND: strSub = TEXT("SUBLANG_GREENLANDIC_GREENLAND"); break;
        }
        break;
    case LANG_GUJARATI: strPrim = TEXT("LANG_GUJARATI");
        SWITCH_SUBLANG()
        {
    case SUBLANG_GUJARATI_INDIA: strSub = TEXT("SUBLANG_GUJARATI_INDIA"); break;
        }
        break;
    case LANG_HAUSA: strPrim = TEXT("LANG_HAUSA");
        SWITCH_SUBLANG()
        {
    case SUBLANG_HAUSA_NIGERIA_LATIN: strSub = TEXT("SUBLANG_HAUSA_NIGERIA_LATIN"); break;
        }
        break;
    case LANG_HEBREW: strPrim = TEXT("LANG_HEBREW");
        SWITCH_SUBLANG()
        {
    case SUBLANG_HEBREW_ISRAEL: strSub = TEXT("SUBLANG_HEBREW_ISRAEL"); break;
        }
        break;
    case LANG_HINDI: strPrim = TEXT("LANG_HINDI");
        SWITCH_SUBLANG()
        {
    case SUBLANG_HINDI_INDIA: strSub = TEXT("SUBLANG_HINDI_INDIA"); break;
        }
        break;
    case LANG_HUNGARIAN: strPrim = TEXT("LANG_HUNGARIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_HUNGARIAN_HUNGARY: strSub = TEXT("SUBLANG_HUNGARIAN_HUNGARY"); break;
        }
        break;
    case LANG_ICELANDIC: strPrim = TEXT("LANG_ICELANDIC");
        SWITCH_SUBLANG()
        {
    case SUBLANG_ICELANDIC_ICELAND: strSub = TEXT("SUBLANG_ICELANDIC_ICELAND"); break;
        }
        break;
    case LANG_IGBO: strPrim = TEXT("LANG_IGBO");
        SWITCH_SUBLANG()
        {
    case SUBLANG_IGBO_NIGERIA: strSub = TEXT("SUBLANG_IGBO_NIGERIA"); break;
        }
        break;
    case LANG_INDONESIAN: strPrim = TEXT("LANG_INDONESIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_INDONESIAN_INDONESIA: strSub = TEXT("SUBLANG_INDONESIAN_INDONESIA"); break;
        }
        break;
    case LANG_INUKTITUT: strPrim = TEXT("LANG_INUKTITUT");
        SWITCH_SUBLANG()
        {
    case SUBLANG_INUKTITUT_CANADA: strSub = TEXT("SUBLANG_INUKTITUT_CANADA"); break;
    case SUBLANG_INUKTITUT_CANADA_LATIN: strSub = TEXT("SUBLANG_INUKTITUT_CANADA_LATIN"); break;
        }
        break;
    case LANG_IRISH: strPrim = TEXT("LANG_IRISH");
        SWITCH_SUBLANG()
        {
    case SUBLANG_IRISH_IRELAND: strSub = TEXT("SUBLANG_IRISH_IRELAND"); break;
        }
        break;
    case LANG_ITALIAN: strPrim = TEXT("LANG_ITALIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_ITALIAN: strSub = TEXT("SUBLANG_ITALIAN"); break;
    case SUBLANG_ITALIAN_SWISS: strSub = TEXT("SUBLANG_ITALIAN_SWISS"); break;
        }
        break;
    case LANG_JAPANESE: strPrim = TEXT("LANG_JAPANESE");
        SWITCH_SUBLANG()
        {
    case SUBLANG_JAPANESE_JAPAN: strSub = TEXT("SUBLANG_JAPANESE_JAPAN"); break;
        }
        break;
    case LANG_KANNADA: strPrim = TEXT("LANG_KANNADA");
        SWITCH_SUBLANG()
        {
    case SUBLANG_KANNADA_INDIA: strSub = TEXT("SUBLANG_KANNADA_INDIA"); break;
        }
        break;
    case LANG_KASHMIRI: strPrim = TEXT("LANG_KASHMIRI");
        SWITCH_SUBLANG()
        {
    case SUBLANG_KASHMIRI_INDIA: strSub = TEXT("SUBLANG_KASHMIRI_INDIA"); break;
        //case SUBLANG_KASHMIRI_SASIA: strSub = TEXT("SUBLANG_KASHMIRI_SASIA"); break; // same as SUBLANG_KASHMIRI_INDIA
        }
        break;
    case LANG_KAZAK: strPrim = TEXT("LANG_KAZAK");
        SWITCH_SUBLANG()
        {
    case SUBLANG_KAZAK_KAZAKHSTAN: strSub = TEXT("SUBLANG_KAZAK_KAZAKHSTAN"); break;
        }
        break;
    case LANG_KHMER: strPrim = TEXT("LANG_KHMER");
        SWITCH_SUBLANG()
        {
    case SUBLANG_KHMER_CAMBODIA: strSub = TEXT("SUBLANG_KHMER_CAMBODIA"); break;
        }
        break;
    case LANG_KICHE: strPrim = TEXT("LANG_KICHE");
        SWITCH_SUBLANG()
        {
    case SUBLANG_KICHE_GUATEMALA: strSub = TEXT("SUBLANG_KICHE_GUATEMALA"); break;
        }
        break;
    case LANG_KINYARWANDA: strPrim = TEXT("LANG_KINYARWANDA");
        SWITCH_SUBLANG()
        {
    case SUBLANG_KINYARWANDA_RWANDA: strSub = TEXT("SUBLANG_KINYARWANDA_RWANDA"); break;
        }
        break;
    case LANG_KONKANI: strPrim = TEXT("LANG_KONKANI");
        SWITCH_SUBLANG()
        {
    case SUBLANG_KONKANI_INDIA: strSub = TEXT("SUBLANG_KONKANI_INDIA"); break;
        }
        break;
    case LANG_KOREAN: strPrim = TEXT("LANG_KOREAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_KOREAN: strSub = TEXT("SUBLANG_KOREAN"); break;
        }
        break;
    case LANG_KYRGYZ: strPrim = TEXT("LANG_KYRGYZ");
        SWITCH_SUBLANG()
        {
    case SUBLANG_KYRGYZ_KYRGYZSTAN: strSub = TEXT("SUBLANG_KYRGYZ_KYRGYZSTAN"); break;
        }
        break;
    case LANG_LAO: strPrim = TEXT("LANG_LAO");
        SWITCH_SUBLANG()
        {
    case SUBLANG_LAO_LAO: strSub = TEXT("SUBLANG_LAO_LAO"); break;
        }
        break;
    case LANG_LATVIAN: strPrim = TEXT("LANG_LATVIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_LATVIAN_LATVIA: strSub = TEXT("SUBLANG_LATVIAN_LATVIA"); break;
        }
        break;
    case LANG_LITHUANIAN: strPrim = TEXT("LANG_LITHUANIAN");
#if (WINVER >= 0x0600) && defined(ENABLE_NEW_LANGS)
        SWITCH_SUBLANG()
        {
    case SUBLANG_LITHUANIAN_LITHUANIA: strSub = TEXT("SUBLANG_LITHUANIAN_LITHUANIA"); break;
        }
#endif
        break;
        //case LANG_LOWER_SORBIAN: strPrim = TEXT("LANG_LOWER_SORBIAN"); // same as LANG_UPPER_SORBIAN
        //    SWITCH_SUBLANG()
        //    {
        //    case SUBLANG_LOWER_SORBIAN_GERMANY: strSub = TEXT("SUBLANG_LOWER_SORBIAN_GERMANY"); break;
        //    }
        //    break;
    case LANG_LUXEMBOURGISH: strPrim = TEXT("LANG_LUXEMBOURGISH");
        SWITCH_SUBLANG()
        {
    case SUBLANG_LUXEMBOURGISH_LUXEMBOURG: strSub = TEXT("SUBLANG_LUXEMBOURGISH_LUXEMBOURG"); break;
        }
        break;
    case LANG_MACEDONIAN: strPrim = TEXT("LANG_MACEDONIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_MACEDONIAN_MACEDONIA: strSub = TEXT("SUBLANG_MACEDONIAN_MACEDONIA"); break;
        }
        break;
    case LANG_MALAY: strPrim = TEXT("LANG_MALAY");
        SWITCH_SUBLANG()
        {
    case SUBLANG_MALAY_MALAYSIA: strSub = TEXT("SUBLANG_MALAY_MALAYSIA"); break;
    case SUBLANG_MALAY_BRUNEI_DARUSSALAM: strSub = TEXT("SUBLANG_MALAY_BRUNEI_DARUSSALAM"); break;
        }
        break;
    case LANG_MALAYALAM: strPrim = TEXT("LANG_MALAYALAM");
        SWITCH_SUBLANG()
        {
    case SUBLANG_MALAYALAM_INDIA: strSub = TEXT("SUBLANG_MALAYALAM_INDIA"); break;
        }
        break;
    case LANG_MALTESE: strPrim = TEXT("LANG_MALTESE");
        SWITCH_SUBLANG()
        {
    case SUBLANG_MALTESE_MALTA: strSub = TEXT("SUBLANG_MALTESE_MALTA"); break;
        }
        break;
    case LANG_MANIPURI: strPrim = TEXT("LANG_MANIPURI");
        break;
    case LANG_MAORI: strPrim = TEXT("LANG_MAORI");
        SWITCH_SUBLANG()
        {
    case SUBLANG_MAORI_NEW_ZEALAND: strSub = TEXT("SUBLANG_MAORI_NEW_ZEALAND"); break;
        }
        break;
    case LANG_MAPUDUNGUN: strPrim = TEXT("LANG_MAPUDUNGUN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_MAPUDUNGUN_CHILE: strSub = TEXT("SUBLANG_MAPUDUNGUN_CHILE"); break;
        }
        break;
    case LANG_MARATHI: strPrim = TEXT("LANG_MARATHI");
        SWITCH_SUBLANG()
        {
    case SUBLANG_MARATHI_INDIA: strSub = TEXT("SUBLANG_MARATHI_INDIA"); break;
        }
        break;
    case LANG_MOHAWK: strPrim = TEXT("LANG_MOHAWK");
        SWITCH_SUBLANG()
        {
    case SUBLANG_MOHAWK_MOHAWK: strSub = TEXT("SUBLANG_MOHAWK_MOHAWK"); break;
        }
        break;
    case LANG_MONGOLIAN: strPrim = TEXT("LANG_MONGOLIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_MONGOLIAN_CYRILLIC_MONGOLIA: strSub = TEXT("SUBLANG_MONGOLIAN_CYRILLIC_MONGOLIA"); break;
    case SUBLANG_MONGOLIAN_PRC: strSub = TEXT("SUBLANG_MONGOLIAN_PRC"); break;
        }
        break;
    case LANG_NEPALI: strPrim = TEXT("LANG_NEPALI");
        SWITCH_SUBLANG()
        {
    case SUBLANG_NEPALI_NEPAL: strSub = TEXT("SUBLANG_NEPALI_NEPAL"); break;
    case SUBLANG_NEPALI_INDIA: strSub = TEXT("SUBLANG_NEPALI_INDIA"); break;
        }
        break;
    case LANG_NORWEGIAN: strPrim = TEXT("LANG_NORWEGIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_NORWEGIAN_BOKMAL: strSub = TEXT("SUBLANG_NORWEGIAN_BOKMAL"); break;
    case SUBLANG_NORWEGIAN_NYNORSK: strSub = TEXT("SUBLANG_NORWEGIAN_NYNORSK"); break;
        }
        break;
    case LANG_OCCITAN: strPrim = TEXT("LANG_OCCITAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_OCCITAN_FRANCE: strSub = TEXT("SUBLANG_OCCITAN_FRANCE"); break;
        }
        break;
        //case LANG_ODIA: strPrim = TEXT("LANG_ODIA");  // same as LANG_ORIYA
        //    break;
    case LANG_ORIYA: strPrim = TEXT("LANG_ORIYA");
        SWITCH_SUBLANG()
        {
    case SUBLANG_ORIYA_INDIA: strSub = TEXT("SUBLANG_ORIYA_INDIA"); break;
        }
        break;
    case LANG_PASHTO: strPrim = TEXT("LANG_PASHTO");
        SWITCH_SUBLANG()
        {
    case SUBLANG_PASHTO_AFGHANISTAN: strSub = TEXT("SUBLANG_PASHTO_AFGHANISTAN"); break;
        }
        break;
    case LANG_PERSIAN: strPrim = TEXT("LANG_PERSIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_PERSIAN_IRAN: strSub = TEXT("SUBLANG_PERSIAN_IRAN"); break;
        }
        break;
    case LANG_POLISH: strPrim = TEXT("LANG_POLISH");
        SWITCH_SUBLANG()
        {
    case SUBLANG_POLISH_POLAND: strSub = TEXT("SUBLANG_POLISH_POLAND"); break;
        }
        break;
    case LANG_PORTUGUESE: strPrim = TEXT("LANG_PORTUGUESE");
        SWITCH_SUBLANG()
        {
    case SUBLANG_PORTUGUESE: strSub = TEXT("SUBLANG_PORTUGUESE"); break;
    case SUBLANG_PORTUGUESE_BRAZILIAN: strSub = TEXT("SUBLANG_PORTUGUESE_BRAZILIAN"); break;
        }
        break;
#ifdef ENABLE_NEW_LANGS
    case LANG_PULAR: strPrim = TEXT("LANG_PULAR"); // same as LANG_FULAH
        SWITCH_SUBLANG()
        {
    case SUBLANG_PULAR_SENEGAL: strSub = TEXT("SUBLANG_PULAR_SENEGAL"); break;
    default: break;
        }
#endif
        break;
    case LANG_PUNJABI: strPrim = TEXT("LANG_PUNJABI");
        SWITCH_SUBLANG()
        {
    case SUBLANG_PUNJABI_INDIA: strSub = TEXT("SUBLANG_PUNJABI_INDIA"); break;
#ifdef ENABLE_NEW_LANGS
    case SUBLANG_PUNJABI_PAKISTAN: strSub = TEXT("SUBLANG_PUNJABI_PAKISTAN"); break;
#endif
    default: break;
        }
        break;
    case LANG_QUECHUA: strPrim = TEXT("LANG_QUECHUA");
        SWITCH_SUBLANG()
        {
    case SUBLANG_QUECHUA_BOLIVIA: strSub = TEXT("SUBLANG_QUECHUA_BOLIVIA"); break;
    case SUBLANG_QUECHUA_ECUADOR: strSub = TEXT("SUBLANG_QUECHUA_ECUADOR"); break;
    case SUBLANG_QUECHUA_PERU: strSub = TEXT("SUBLANG_QUECHUA_PERU"); break;
        }
        break;
    case LANG_ROMANIAN: strPrim = TEXT("LANG_ROMANIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_ROMANIAN_ROMANIA: strSub = TEXT("SUBLANG_ROMANIAN_ROMANIA"); break;
        }
        break;
    case LANG_ROMANSH: strPrim = TEXT("LANG_ROMANSH");
        SWITCH_SUBLANG()
        {
    case SUBLANG_ROMANSH_SWITZERLAND: strSub = TEXT("SUBLANG_ROMANSH_SWITZERLAND"); break;
        }
        break;
    case LANG_RUSSIAN: strPrim = TEXT("LANG_RUSSIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_RUSSIAN_RUSSIA: strSub = TEXT("SUBLANG_RUSSIAN_RUSSIA"); break;
        }
        break;
#ifdef ENABLE_NEW_LANGS
    case LANG_SAKHA: strPrim = TEXT("LANG_SAKHA");
        SWITCH_SUBLANG()
        {
    case SUBLANG_SAKHA_RUSSIA: strSub = TEXT("SUBLANG_SAKHA_RUSSIA"); break;
    default: break;
        }
        break;
#endif
    case LANG_SAMI: strPrim = TEXT("LANG_SAMI");
        SWITCH_SUBLANG()
        {
    case SUBLANG_SAMI_NORTHERN_NORWAY: strSub = TEXT("SUBLANG_SAMI_NORTHERN_NORWAY"); break;
    case SUBLANG_SAMI_NORTHERN_SWEDEN: strSub = TEXT("SUBLANG_SAMI_NORTHERN_SWEDEN"); break;
    case SUBLANG_SAMI_NORTHERN_FINLAND: strSub = TEXT("SUBLANG_SAMI_NORTHERN_FINLAND"); break;
    case SUBLANG_SAMI_LULE_NORWAY: strSub = TEXT("SUBLANG_SAMI_LULE_NORWAY"); break;
    case SUBLANG_SAMI_LULE_SWEDEN: strSub = TEXT("SUBLANG_SAMI_LULE_SWEDEN"); break;
    case SUBLANG_SAMI_SOUTHERN_NORWAY: strSub = TEXT("SUBLANG_SAMI_SOUTHERN_NORWAY"); break;
    case SUBLANG_SAMI_SOUTHERN_SWEDEN: strSub = TEXT("SUBLANG_SAMI_SOUTHERN_SWEDEN"); break;
    case SUBLANG_SAMI_SKOLT_FINLAND: strSub = TEXT("SUBLANG_SAMI_SKOLT_FINLAND"); break;
    case SUBLANG_SAMI_INARI_FINLAND: strSub = TEXT("SUBLANG_SAMI_INARI_FINLAND"); break;
        }
        break;
    case LANG_SANSKRIT: strPrim = TEXT("LANG_SANSKRIT");
        SWITCH_SUBLANG()
        {
    case SUBLANG_SANSKRIT_INDIA: strSub = TEXT("SUBLANG_SANSKRIT_INDIA"); break;
        }
        break;
#ifdef ENABLE_NEW_LANGS
    case LANG_SCOTTISH_GAELIC: strPrim = TEXT("LANG_SCOTTISH_GAELIC");
        SWITCH_SUBLANG()
        {
    case SUBLANG_SCOTTISH_GAELIC: strSub = TEXT("SUBLANG_SCOTTISH_GAELIC"); break;
    default: break;
        }
        break;
#endif
        //case LANG_SERBIAN: strPrim = TEXT("LANG_SERBIAN"); // same as LANG_BOSNIAN
        //    SWITCH_SUBLANG()
        //    {
        //    case SUBLANG_SERBIAN_LATIN: strSub = TEXT("SUBLANG_SERBIAN_LATIN"); break;
        //    case SUBLANG_SERBIAN_CYRILLIC: strSub = TEXT("SUBLANG_SERBIAN_CYRILLIC"); break;
        //    case SUBLANG_SERBIAN_BOSNIA_HERZEGOVINA_LATIN: strSub = TEXT("SUBLANG_SERBIAN_BOSNIA_HERZEGOVINA_LATIN"); break;
        //    case SUBLANG_SERBIAN_BOSNIA_HERZEGOVINA_CYRILLIC: strSub = TEXT("SUBLANG_SERBIAN_BOSNIA_HERZEGOVINA_CYRILLIC"); break;
        //    case SUBLANG_SERBIAN_MONTENEGRO_LATIN: strSub = TEXT("SUBLANG_SERBIAN_MONTENEGRO_LATIN"); break;
        //    case SUBLANG_SERBIAN_MONTENEGRO_CYRILLIC: strSub = TEXT("SUBLANG_SERBIAN_MONTENEGRO_CYRILLIC"); break;
        //    case SUBLANG_SERBIAN_SERBIA_LATIN: strSub = TEXT("SUBLANG_SERBIAN_SERBIA_LATIN"); break;
        //    case SUBLANG_SERBIAN_SERBIA_CYRILLIC: strSub = TEXT("SUBLANG_SERBIAN_SERBIA_CYRILLIC"); break;
        //    }
        //    break;
    case LANG_SERBIAN_NEUTRAL: strPrim = TEXT("LANG_SERBIAN_NEUTRAL");
        SWITCH_SUBLANG()
        {
    case SUBLANG_SERBIAN_LATIN: strSub = TEXT("SUBLANG_SERBIAN_LATIN"); break;
    case SUBLANG_SERBIAN_CYRILLIC: strSub = TEXT("SUBLANG_SERBIAN_CYRILLIC"); break;
    case SUBLANG_SERBIAN_BOSNIA_HERZEGOVINA_LATIN: strSub = TEXT("SUBLANG_SERBIAN_BOSNIA_HERZEGOVINA_LATIN"); break;
    case SUBLANG_SERBIAN_BOSNIA_HERZEGOVINA_CYRILLIC: strSub = TEXT("SUBLANG_SERBIAN_BOSNIA_HERZEGOVINA_CYRILLIC"); break;
#ifdef ENABLE_NEW_LANGS
    case SUBLANG_SERBIAN_MONTENEGRO_LATIN: strSub = TEXT("SUBLANG_SERBIAN_MONTENEGRO_LATIN"); break;
    case SUBLANG_SERBIAN_MONTENEGRO_CYRILLIC: strSub = TEXT("SUBLANG_SERBIAN_MONTENEGRO_CYRILLIC"); break;
    case SUBLANG_SERBIAN_SERBIA_LATIN: strSub = TEXT("SUBLANG_SERBIAN_SERBIA_LATIN"); break;
    case SUBLANG_SERBIAN_SERBIA_CYRILLIC: strSub = TEXT("SUBLANG_SERBIAN_SERBIA_CYRILLIC"); break;
#endif
        }
        break;
    case LANG_SINDHI: strPrim = TEXT("LANG_SINDHI");
        SWITCH_SUBLANG()
        {
    case SUBLANG_SINDHI_INDIA: strSub = TEXT("SUBLANG_SINDHI_INDIA"); break;
    case SUBLANG_SINDHI_AFGHANISTAN: strSub = TEXT("SUBLANG_SINDHI_AFGHANISTAN"); break;
        //case SUBLANG_SINDHI_PAKISTAN: strSub = TEXT("SUBLANG_SINDHI_PAKISTAN"); break; // same as SUBLANG_SINDHI_AFGHANISTAN
        }
        break;
    case LANG_SINHALESE: strPrim = TEXT("LANG_SINHALESE");
        SWITCH_SUBLANG()
        {
    case SUBLANG_SINHALESE_SRI_LANKA: strSub = TEXT("SUBLANG_SINHALESE_SRI_LANKA"); break;
        }
        break;
    case LANG_SLOVAK: strPrim = TEXT("LANG_SLOVAK");
        SWITCH_SUBLANG()
        {
    case SUBLANG_SLOVAK_SLOVAKIA: strSub = TEXT("SUBLANG_SLOVAK_SLOVAKIA"); break;
        }
        break;
    case LANG_SLOVENIAN: strPrim = TEXT("LANG_SLOVENIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_SLOVENIAN_SLOVENIA: strSub = TEXT("SUBLANG_SLOVENIAN_SLOVENIA"); break;
        }
        break;
    case LANG_SOTHO: strPrim = TEXT("LANG_SOTHO");
        SWITCH_SUBLANG()
        {
    case SUBLANG_SOTHO_NORTHERN_SOUTH_AFRICA: strSub = TEXT("SUBLANG_SOTHO_NORTHERN_SOUTH_AFRICA"); break;
        }
        break;
    case LANG_SPANISH: strPrim = TEXT("LANG_SPANISH");
        SWITCH_SUBLANG()
        {
    case SUBLANG_SPANISH: strSub = TEXT("SUBLANG_SPANISH"); break;
    case SUBLANG_SPANISH_MEXICAN: strSub = TEXT("SUBLANG_SPANISH_MEXICAN"); break;
    case SUBLANG_SPANISH_MODERN: strSub = TEXT("SUBLANG_SPANISH_MODERN"); break;
    case SUBLANG_SPANISH_GUATEMALA: strSub = TEXT("SUBLANG_SPANISH_GUATEMALA"); break;
    case SUBLANG_SPANISH_COSTA_RICA: strSub = TEXT("SUBLANG_SPANISH_COSTA_RICA"); break;
    case SUBLANG_SPANISH_PANAMA: strSub = TEXT("SUBLANG_SPANISH_PANAMA"); break;
    case SUBLANG_SPANISH_DOMINICAN_REPUBLIC: strSub = TEXT("SUBLANG_SPANISH_DOMINICAN_REPUBLIC"); break;
    case SUBLANG_SPANISH_VENEZUELA: strSub = TEXT("SUBLANG_SPANISH_VENEZUELA"); break;
    case SUBLANG_SPANISH_COLOMBIA: strSub = TEXT("SUBLANG_SPANISH_COLOMBIA"); break;
    case SUBLANG_SPANISH_PERU: strSub = TEXT("SUBLANG_SPANISH_PERU"); break;
    case SUBLANG_SPANISH_ARGENTINA: strSub = TEXT("SUBLANG_SPANISH_ARGENTINA"); break;
    case SUBLANG_SPANISH_ECUADOR: strSub = TEXT("SUBLANG_SPANISH_ECUADOR"); break;
    case SUBLANG_SPANISH_CHILE: strSub = TEXT("SUBLANG_SPANISH_CHILE"); break;
    case SUBLANG_SPANISH_URUGUAY: strSub = TEXT("SUBLANG_SPANISH_URUGUAY"); break;
    case SUBLANG_SPANISH_PARAGUAY: strSub = TEXT("SUBLANG_SPANISH_PARAGUAY"); break;
    case SUBLANG_SPANISH_BOLIVIA: strSub = TEXT("SUBLANG_SPANISH_BOLIVIA"); break;
    case SUBLANG_SPANISH_EL_SALVADOR: strSub = TEXT("SUBLANG_SPANISH_EL_SALVADOR"); break;
    case SUBLANG_SPANISH_HONDURAS: strSub = TEXT("SUBLANG_SPANISH_HONDURAS"); break;
    case SUBLANG_SPANISH_NICARAGUA: strSub = TEXT("SUBLANG_SPANISH_NICARAGUA"); break;
    case SUBLANG_SPANISH_PUERTO_RICO: strSub = TEXT("SUBLANG_SPANISH_PUERTO_RICO"); break;
    case SUBLANG_SPANISH_US: strSub = TEXT("SUBLANG_SPANISH_US"); break;
        }
        break;
    case LANG_SWAHILI: strPrim = TEXT("LANG_SWAHILI");
        SWITCH_SUBLANG()
        {
    case SUBLANG_SWAHILI_KENYA: strSub = TEXT("SUBLANG_SWAHILI_KENYA"); break;
        }
        break;
    case LANG_SWEDISH: strPrim = TEXT("LANG_SWEDISH");
        SWITCH_SUBLANG()
        {
            //case SUBLANG_SWEDISH_SWEDEN: strSub = TEXT("SUBLANG_SWEDISH_SWEDEN"); break; // same as SUBLANG_SWEDISH
    case SUBLANG_SWEDISH: strSub = TEXT("SUBLANG_SWEDISH"); break;
    case SUBLANG_SWEDISH_FINLAND: strSub = TEXT("SUBLANG_SWEDISH_FINLAND"); break;
        }
        break;
    case LANG_SYRIAC: strPrim = TEXT("LANG_SYRIAC");
#if defined(ENABLE_NEW_LANGS)
        SWITCH_SUBLANG()
        {
    case SUBLANG_SYRIAC: strSub = TEXT("SUBLANG_SYRIAC"); break;
        }
#endif
        break;
    case LANG_TAJIK: strPrim = TEXT("LANG_TAJIK");
        SWITCH_SUBLANG()
        {
    case SUBLANG_TAJIK_TAJIKISTAN: strSub = TEXT("SUBLANG_TAJIK_TAJIKISTAN"); break;
        }
        break;
    case LANG_TAMAZIGHT: strPrim = TEXT("LANG_TAMAZIGHT");
        SWITCH_SUBLANG()
        {
    case SUBLANG_TAMAZIGHT_ALGERIA_LATIN: strSub = TEXT("SUBLANG_TAMAZIGHT_ALGERIA_LATIN   "); break;
#ifdef ENABLE_NEW_LANGS
    case SUBLANG_TAMAZIGHT_MOROCCO_TIFINAGH: strSub = TEXT("SUBLANG_TAMAZIGHT_MOROCCO_TIFINAGH"); break;
#endif
        }
        break;
    case LANG_TAMIL: strPrim = TEXT("LANG_TAMIL");
        SWITCH_SUBLANG()
        {
    case SUBLANG_TAMIL_INDIA: strSub = TEXT("SUBLANG_TAMIL_INDIA"); break;
#ifdef ENABLE_NEW_LANGS
    case SUBLANG_TAMIL_SRI_LANKA: strSub = TEXT("SUBLANG_TAMIL_SRI_LANKA"); break;
#endif
    default: break;
        }
        break;
    case LANG_TATAR: strPrim = TEXT("LANG_TATAR");
        SWITCH_SUBLANG()
        {
    case SUBLANG_TATAR_RUSSIA: strSub = TEXT("SUBLANG_TATAR_RUSSIA"); break;
        }
        break;
    case LANG_TELUGU: strPrim = TEXT("LANG_TELUGU");
        SWITCH_SUBLANG()
        {
    case SUBLANG_TELUGU_INDIA: strSub = TEXT("SUBLANG_TELUGU_INDIA"); break;
        }
        break;
    case LANG_THAI: strPrim = TEXT("LANG_THAI");
        SWITCH_SUBLANG()
        {
    case SUBLANG_THAI_THAILAND: strSub = TEXT("SUBLANG_THAI_THAILAND"); break;
        }
        break;
    case LANG_TIBETAN: strPrim = TEXT("LANG_TIBETAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_TIBETAN_PRC: strSub = TEXT("SUBLANG_TIBETAN_PRC"); break;
#if defined(ENABLE_NEW_LANGS)
    case SUBLANG_TIBETAN_BHUTAN: strSub = TEXT("SUBLANG_TIBETAN_BHUTAN"); break;
#endif
        }
        break;
    case LANG_TIGRIGNA: strPrim = TEXT("LANG_TIGRIGNA"); // same as LANG_TIGRINYA
        SWITCH_SUBLANG()
        {
    case SUBLANG_TIGRIGNA_ERITREA: strSub = TEXT("SUBLANG_TIGRIGNA_ERITREA"); break;
    default: break;
        }
        break;
        //case LANG_TIGRINYA: strPrim = TEXT("LANG_TIGRINYA"); // same as LANG_TIGRIGNA
        //    SWITCH_SUBLANG()
        //    {
        //    case SUBLANG_TIGRINYA_ERITREA: strSub = TEXT("SUBLANG_TIGRINYA_ERITREA"); break;
        //    case SUBLANG_TIGRINYA_ETHIOPIA: strSub = TEXT("SUBLANG_TIGRINYA_ETHIOPIA"); break;
        //    }
        //    break;
    case LANG_TSWANA: strPrim = TEXT("LANG_TSWANA");
        SWITCH_SUBLANG()
        {
#ifdef ENABLE_NEW_LANGS
    case SUBLANG_TSWANA_BOTSWANA: strSub = TEXT("SUBLANG_TSWANA_BOTSWANA"); break;
#endif
    case SUBLANG_TSWANA_SOUTH_AFRICA: strSub = TEXT("SUBLANG_TSWANA_SOUTH_AFRICA"); break;
        }
        break;
    case LANG_TURKISH: strPrim = TEXT("LANG_TURKISH");
        SWITCH_SUBLANG()
        {
    case SUBLANG_TURKISH_TURKEY: strSub = TEXT("SUBLANG_TURKISH_TURKEY"); break;
        }
        break;
    case LANG_TURKMEN: strPrim = TEXT("LANG_TURKMEN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_TURKMEN_TURKMENISTAN: strSub = TEXT("SUBLANG_TURKMEN_TURKMENISTAN"); break;
        }
        break;
    case LANG_UIGHUR: strPrim = TEXT("LANG_UIGHUR");
        SWITCH_SUBLANG()
        {
    case SUBLANG_UIGHUR_PRC: strSub = TEXT("SUBLANG_UIGHUR_PRC"); break;
        }
        break;
    case LANG_UKRAINIAN: strPrim = TEXT("LANG_UKRAINIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_UKRAINIAN_UKRAINE: strSub = TEXT("SUBLANG_UKRAINIAN_UKRAINE"); break;
        }
        break;
    case LANG_UPPER_SORBIAN: strPrim = TEXT("LANG_UPPER_SORBIAN");
        SWITCH_SUBLANG()
        {
    case SUBLANG_UPPER_SORBIAN_GERMANY: strSub = TEXT("SUBLANG_UPPER_SORBIAN_GERMANY"); break;
        }
        break;
    case LANG_URDU: strPrim = TEXT("LANG_URDU");
        SWITCH_SUBLANG()
        {
    case SUBLANG_URDU_PAKISTAN: strSub = TEXT("SUBLANG_URDU_PAKISTAN"); break;
    case SUBLANG_URDU_INDIA: strSub = TEXT("SUBLANG_URDU_INDIA"); break;
        }
        break;
    case LANG_UZBEK: strPrim = TEXT("LANG_UZBEK");
        SWITCH_SUBLANG()
        {
    case SUBLANG_UZBEK_LATIN: strSub = TEXT("SUBLANG_UZBEK_LATIN"); break;
    case SUBLANG_UZBEK_CYRILLIC: strSub = TEXT("SUBLANG_UZBEK_CYRILLIC"); break;
        }
        break;
        //case LANG_VALENCIAN: strPrim = TEXT("LANG_VALENCIAN"); // same as LANG_CATALAN
        //    SWITCH_SUBLANG()
        //    {
        //    case SUBLANG_VALENCIAN_VALENCIA: strSub = TEXT("SUBLANG_VALENCIAN_VALENCIA"); break;
        //    }
        //    break;
    case LANG_VIETNAMESE: strPrim = TEXT("LANG_VIETNAMESE");
        SWITCH_SUBLANG()
        {
    case SUBLANG_VIETNAMESE_VIETNAM: strSub = TEXT("SUBLANG_VIETNAMESE_VIETNAM"); break;
        }
        break;
    case LANG_WELSH: strPrim = TEXT("LANG_WELSH");
        SWITCH_SUBLANG()
        {
    case SUBLANG_WELSH_UNITED_KINGDOM: strSub = TEXT("SUBLANG_WELSH_UNITED_KINGDOM"); break;
        }
        break;
    case LANG_WOLOF: strPrim = TEXT("LANG_WOLOF");
        SWITCH_SUBLANG()
        {
    case SUBLANG_WOLOF_SENEGAL: strSub = TEXT("SUBLANG_WOLOF_SENEGAL"); break;
        }
        break;
    case LANG_XHOSA: strPrim = TEXT("LANG_XHOSA");
        SWITCH_SUBLANG()
        {
    case SUBLANG_XHOSA_SOUTH_AFRICA: strSub = TEXT("SUBLANG_XHOSA_SOUTH_AFRICA"); break;
        }
        break;
        //case LANG_YAKUT: strPrim = TEXT("LANG_YAKUT"); // same as LANG_SAKHA
        //    SWITCH_SUBLANG()
        //    {
        //    case SUBLANG_YAKUT_RUSSIA: strSub = TEXT("SUBLANG_YAKUT_RUSSIA"); break;
        //    }
        //    break;
    case LANG_YI: strPrim = TEXT("LANG_YI");
        SWITCH_SUBLANG()
        {
    case SUBLANG_YI_PRC: strSub = TEXT("SUBLANG_YI_PRC"); break;
        }
        break;
    case LANG_YORUBA: strPrim = TEXT("LANG_YORUBA");
        SWITCH_SUBLANG()
        {
    case SUBLANG_YORUBA_NIGERIA: strSub = TEXT("SUBLANG_YORUBA_NIGERIA"); break;
        }
        break;
    case LANG_ZULU: strPrim = TEXT("LANG_ZULU");
        SWITCH_SUBLANG()
        {
    case SUBLANG_ZULU_SOUTH_AFRICA: strSub = TEXT("SUBLANG_ZULU_SOUTH_AFRICA"); break;
        }
        break;
    default:
        break;
    }

    TCHAR szText[32];
    if (strPrim.empty())
    {
        StringCchPrintf(szText, _countof(szText), TEXT("0x%04X"), PRIMARYLANGID(langid));
        strPrim = szText;
    }

    if (bOldStyle)
        strSub.clear();

    // sub-language
    if (strSub.empty())
    {
        switch (SUBLANGID(langid))
        {
        case SUBLANG_NEUTRAL: strSub = TEXT("SUBLANG_NEUTRAL"); break;
        case SUBLANG_DEFAULT: strSub = TEXT("SUBLANG_DEFAULT"); break;
        case SUBLANG_SYS_DEFAULT: strSub = TEXT("SUBLANG_SYS_DEFAULT"); break;
        case SUBLANG_CUSTOM_DEFAULT: strSub = TEXT("SUBLANG_CUSTOM_DEFAULT"); break;
        case SUBLANG_CUSTOM_UNSPECIFIED: strSub = TEXT("SUBLANG_CUSTOM_UNSPECIFIED"); break;
        case SUBLANG_UI_CUSTOM_DEFAULT: strSub = TEXT("SUBLANG_UI_CUSTOM_DEFAULT"); break;
        default:
            break;
        }
    }

    // sub-language
    if (strSub.empty())
    {
        StringCchPrintf(szText, _countof(szText), TEXT("0x%04X"), SUBLANGID(langid));
        strSub = szText;
    }
#undef SWITCH_SUBLANG

    // output the LANGUAGE statement
    MString str = TEXT("LANGUAGE ");
    str += strPrim;
    str += TEXT(", ");
    str += strSub;
    str += TEXT("\r\n");

    return str;
}

MStringW
GetResTypeEncoding(const MIdOrString& type)
{
    MStringW name;

    if (type.m_id)
    {
        name = g_db.GetName(L"RESOURCE", type.m_id);
        if (name.empty())
            name = mstr_dec_word(type.m_id);
    }
    else
    {
        name = type.str();
    }

    auto it = g_settings.encoding_map.find(name);
    if (it != g_settings.encoding_map.end())
        return it->second;

    return L"";
}
void ExportRC(LPCWSTR file, LPCWSTR exportpath, bool pseudolocalize) {

    g_db.LoadFromFile(L".\\constants.txt");


    g_db.m_map[L"CTRLID"].emplace_back(L"IDC_STATIC", (WORD)-1);

    DoLoadFile(file, 0, 0);
    WCHAR resh[MAX_PATH] = L"";
    DoExport(exportpath, resh,pseudolocalize);
    


}