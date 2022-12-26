// mcdx.hpp --- Message Compiler by katahiromz
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


#if defined(_WIN32) && !defined(WONVER)
#include "MProcessMaker.hpp"
#endif
#include "MString.hpp"
#include "MacroParser.hpp"
#include "MessageRes.hpp"
#include<fstream>
#include "ResHeader.hpp"
#include <cctype>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef RT_MESSAGETABLE
#define RT_MESSAGETABLE     MAKEINTRESOURCE(11)
#endif

////////////////////////////////////////////////////////////////////////////

#ifndef _countof
#define _countof(array)     (sizeof(array) / sizeof(array[0]))
#endif

enum EXITCODE
{
    EXITCODE_SUCCESS = 0,
    EXITCODE_INVALID_ARGUMENT,
    EXITCODE_FAIL_TO_PREPROCESS,
    EXITCODE_SYNTAX_ERROR,
    EXITCODE_CANNOT_OPEN,
    EXITCODE_CANNOT_WRITE,
    EXITCODE_INVALID_DATA,
    EXITCODE_NOT_FOUND_CPP,
    EXITCODE_NOT_FOUND_WINDRES,
    EXITCODE_NOT_SUPPORTED_YET,
    EXITCODE_CANT_MAKE_TEMP
};


////////////////////////////////////////////////////////////////////////////


wchar_t* g_input_file = NULL;
wchar_t* g_output_file = NULL;
const wchar_t* g_inp_format = NULL;
const wchar_t* g_out_format = NULL;
int g_nLineNo = 0;

LANGID g_langid = 0;
int g_value = 0;

typedef std::map<LANGID, MessageRes> msg_tables_type;
msg_tables_type g_msg_tables;


int syntax_error(void)
{
    assert(0);
    return EXITCODE_SYNTAX_ERROR;
}


bool do_directive_line(char*& ptr)
{
    // # line "file"
    char* ptr1 = ptr;
    while (mchr_is_digit(*ptr))
    {
        ++ptr;
    }
    char* ptr2 = ptr;
    ptr = mstr_skip_space(ptr);
    char* ptr3 = ptr;
    while (*ptr)
    {
        ++ptr;
    }
    *ptr2 = 0;

    g_nLineNo = strtol(ptr1, NULL, 0) - 1;

    std::string file = ptr3;
    mstr_unquote(file);

    return true;
}

int do_mode_1(char*& ptr, int& nMode, bool& do_retry)
{
    ptr = mstr_skip_space(ptr);
    if (*ptr == '{')
    {
        nMode = 2;
        ++ptr;
    }
    else if (*ptr == '}')
    {
        return syntax_error();
    }
    else if (memcmp(ptr, "BEGIN", 5) == 0 &&
        (ptr[5] == 0 || mchr_is_space(ptr[5])))
    {
        nMode = 2;
        ptr += 5;
    }
    ptr = mstr_skip_space(ptr);
    if (nMode != 2)
    {
        if (*ptr && !mchr_is_digit(*ptr))
        {
            return syntax_error();
        }
    }
    return EXITCODE_SUCCESS;
}

int do_mode_2(char*& ptr, int& nMode, bool& do_retry)
{
    ptr = mstr_skip_space(ptr);
    if (*ptr == '{')
    {
        return syntax_error();
    }
    else if (*ptr == '}')
    {
        ++ptr;
        nMode = 0;
        do_retry = true;
        return EXITCODE_SUCCESS;
    }
    else if (memcmp(ptr, "END", 3) == 0 &&
        (ptr[3] == 0 || mchr_is_space(ptr[3])))
    {
        ptr += 3;
        nMode = 0;
        do_retry = true;
        return EXITCODE_SUCCESS;
    }
    if (*ptr)
    {
        // get number string
        char* ptr0 = ptr;
        while (*ptr && *ptr != ',' && *ptr != '\"')
        {
            ++ptr;
        }
        char* ptr1 = ptr;
        MStringA str(ptr0, ptr1);

        // parse
        using namespace MacroParser;
        StringScanner scanner(str);
        TokenStream stream(scanner);
        stream.read_tokens();
        Parser parser(stream);
        if (parser.parse())
        {
            if (!eval_int(parser.ast(), g_value))
            {
                return syntax_error();
            }
        }
        else
        {
            return syntax_error();
        }

        if (*ptr == ',')
        {
            ++ptr;
            nMode = 3;
            do_retry = true;
            return EXITCODE_SUCCESS;
        }
        else if (*ptr == '\"')
        {
            nMode = 3;
            do_retry = true;
            return EXITCODE_SUCCESS;
        }
        else if (*ptr == 0)
        {
            nMode = 3;
        }
        else
        {
            return syntax_error();
        }
    }

    return EXITCODE_SUCCESS;
}

int do_mode_3(char*& ptr, int& nMode, bool& do_retry)
{
    ptr = mstr_skip_space(ptr);
    if (*ptr == ',')
    {
        ++ptr;
    }
    ptr = mstr_skip_space(ptr);
    if (*ptr == '"')
    {
        MStringA str = ptr;
        mstr_unquote(str);

        MStringW wstr(MAnsiToWide(CP_UTF8, str.c_str()).c_str());
        g_msg_tables[g_langid].m_map[(DWORD)g_value] = wstr;

        const char* ptr0 = ptr;
        guts_quote(str, ptr0);
        ptr = const_cast<char*>(ptr0);

        nMode = 2;
        do_retry = true;
        return EXITCODE_SUCCESS;
    }

    if (*ptr != 0)
    {
        return syntax_error();
    }

    return EXITCODE_SUCCESS;
}

int do_directive(char*& ptr)
{
    ++ptr;
    ptr = mstr_skip_space(ptr);
    if (mchr_is_digit(*ptr))
    {
        do_directive_line(ptr);
    }
    else if (memcmp(ptr, "pragma", 6) == 0)
    {
        // #pragma
        ptr += 6;
        ptr = mstr_skip_space(ptr);
        char* ptr2 = ptr;
        if (memcmp(ptr, "pack", 4) == 0)
        {
            // #pragma pack...
        }
        else if (memcmp(ptr, "code_page", 9) == 0)
        {
            ptr += 9;
            ptr = mstr_skip_space(ptr);
            if (*ptr == '(')
            {
                ++ptr;
                ptr = mstr_skip_space(ptr);
                // #pragma code_page(...)
                uint16_t wCodePage = 0;
                if (mchr_is_digit(*ptr))
                {
                    wCodePage = uint16_t(strtol(ptr, NULL, 0));
                }
                while (mchr_is_alnum(*ptr))
                {
                    ++ptr;
                }
                ptr = mstr_skip_space(ptr);
                if (*ptr == ')')
                {
                    ++ptr;
                }
                else
                {
                }
            }
            else
            {
            }
        }
        else
        {
        }
    }

    return EXITCODE_SUCCESS;
}

int eat_output(const std::string& output)
{
    g_msg_tables.clear();

    std::vector<std::string> lines;
    mstr_split(lines, output, "\n");

    for (size_t i = 0; i < lines.size(); ++i)
    {
        mstr_trim(lines[i]);
    }

    // parse lines
    int nMode = 0;
    uint8_t bPrimLang = 0, bSubLang = 0;
    for (size_t i = 0; i < lines.size(); ++i, ++g_nLineNo)
    {
        std::string& line = lines[i];
        if (line.empty())
            continue;
        char* ptr = &line[0];
        if (*ptr == '#')
        {
            if (int ret = do_directive(ptr))
                return ret;

            continue;
        }
        else if (memcmp("LANGUAGE", ptr, 8) == 0 &&
            (ptr[8] == 0 || mchr_is_space(ptr[8])))
        {
            // LANGUAGE (primary), (sublang)
            ptr += 8;
            nMode = -1;
        }
    retry:
        if (nMode == -1 && *ptr)    // after LANGUAGE
        {
            ptr = mstr_skip_space(ptr);
            if (mchr_is_digit(*ptr))
            {
                nMode = -2;
            }
        }
        if (nMode == -2 && *ptr)    // expect PRIMARYLANGID
        {
            ptr = mstr_skip_space(ptr);
            char* ptr0 = ptr;
            while (mchr_is_alnum(*ptr))
            {
                ++ptr;
            }
            if (mchr_is_digit(*ptr0))
            {
                bPrimLang = (uint8_t)strtoul(ptr0, NULL, 0);
                nMode = -3;
            }
            else if (*ptr)
            {
                return syntax_error();
            }
        }
        if (nMode == -3 && *ptr)    // expect comma
        {
            ptr = mstr_skip_space(ptr);
            if (*ptr == ',')
            {
                ++ptr;
                nMode = -4;
            }
        }
        if (nMode == -4 && *ptr)    // expect SUBLANGID
        {
            ptr = mstr_skip_space(ptr);
            if (mchr_is_digit(*ptr))
            {
                bSubLang = (uint8_t)strtoul(ptr, NULL, 0);
                g_langid = MAKELANGID(bPrimLang, bSubLang);
                nMode = 0;
            }
            else if (*ptr)
            {
                return syntax_error();
            }
        }
        if (nMode == 0 && *ptr) // out of MESSAGETABLEDX { ... }
        {
            ptr = mstr_skip_space(ptr);
            if (memcmp("MESSAGETABLEDX", ptr, 14) == 0 &&
                (mchr_is_space(ptr[14]) || ptr[14] == 0 || ptr[14] == '{'))  // }
            {
                nMode = 1;
                ptr += 14;
                ptr = mstr_skip_space(ptr);
            }
        }
        if (nMode == 1 && *ptr) // after MESSAGETABLEDX
        {
            bool do_retry = false;
            if (int ret = do_mode_1(ptr, nMode, do_retry))
                return ret;
            if (do_retry)
                goto retry;
        }
        if (nMode == 2 && *ptr) // in MESSAGETABLEDX { ... }
        {
            bool do_retry = false;
            if (int ret = do_mode_2(ptr, nMode, do_retry))
                return ret;
            if (do_retry)
                goto retry;
        }
        if (nMode == 3 && *ptr)
        {
            bool do_retry = false;
            if (int ret = do_mode_3(ptr, nMode, do_retry))
                return ret;
            if (do_retry)
                goto retry;
        }
    }
    if (nMode != 0)
    {
        return syntax_error();
    }

    return EXITCODE_SUCCESS;
}
int eat_output(std::vector<std::string>& lines)
{
    g_msg_tables.clear();

    for (size_t i = 0; i < lines.size(); ++i)
    {
        mstr_trim(lines[i]);
    }

    // parse lines
    int nMode = 0;
    uint8_t bPrimLang = 0, bSubLang = 0;
    for (size_t i = 0; i < lines.size(); ++i, ++g_nLineNo)
    {
        std::string& line = lines[i];
        if (line.empty())
            continue;
        char* ptr = &line[0];
        if (*ptr == '#')
        {
            if (int ret = do_directive(ptr))
                return ret;

            continue;
        }
        else if (memcmp("LANGUAGE", ptr, 8) == 0 &&
            (ptr[8] == 0 || mchr_is_space(ptr[8])))
        {
            // LANGUAGE (primary), (sublang)
            ptr += 8;
            nMode = -1;
        }
    retry:
        if (nMode == -1 && *ptr)    // after LANGUAGE
        {
            ptr = mstr_skip_space(ptr);
            if (mchr_is_digit(*ptr))
            {
                nMode = -2;
            }
        }
        if (nMode == -2 && *ptr)    // expect PRIMARYLANGID
        {
            ptr = mstr_skip_space(ptr);
            char* ptr0 = ptr;
            while (mchr_is_alnum(*ptr))
            {
                ++ptr;
            }
            if (mchr_is_digit(*ptr0))
            {
                bPrimLang = (uint8_t)strtoul(ptr0, NULL, 0);
                nMode = -3;
            }
            else if (*ptr)
            {
                return syntax_error();
            }
        }
        if (nMode == -3 && *ptr)    // expect comma
        {
            ptr = mstr_skip_space(ptr);
            if (*ptr == ',')
            {
                ++ptr;
                nMode = -4;
            }
        }
        if (nMode == -4 && *ptr)    // expect SUBLANGID
        {
            ptr = mstr_skip_space(ptr);
            if (mchr_is_digit(*ptr))
            {
                bSubLang = (uint8_t)strtoul(ptr, NULL, 0);
                g_langid = MAKELANGID(bPrimLang, bSubLang);
                nMode = 0;
            }
            else if (*ptr)
            {
                return syntax_error();
            }
        }
        if (nMode == 0 && *ptr) // out of MESSAGETABLEDX { ... }
        {
            ptr = mstr_skip_space(ptr);
            if (memcmp("MESSAGETABLEDX", ptr, 14) == 0 &&
                (mchr_is_space(ptr[14]) || ptr[14] == 0 || ptr[14] == '{'))  // }
            {
                nMode = 1;
                ptr += 14;
                ptr = mstr_skip_space(ptr);
            }
        }
        if (nMode == 1 && *ptr) // after MESSAGETABLEDX
        {
            bool do_retry = false;
            if (int ret = do_mode_1(ptr, nMode, do_retry))
                return ret;
            if (do_retry)
                goto retry;
        }
        if (nMode == 2 && *ptr) // in MESSAGETABLEDX { ... }
        {
            bool do_retry = false;
            if (int ret = do_mode_2(ptr, nMode, do_retry))
                return ret;
            if (do_retry)
                goto retry;
        }
        if (nMode == 3 && *ptr)
        {
            bool do_retry = false;
            if (int ret = do_mode_3(ptr, nMode, do_retry))
                return ret;
            if (do_retry)
                goto retry;
        }
    }
    if (nMode != 0)
    {
        return syntax_error();
    }

    return EXITCODE_SUCCESS;
}

int save_rc(const wchar_t* output_file, bool pseudolocalize=false)
{
    FILE* fp;
    if (output_file)
    {
        fp = _wfopen(output_file, L"wb");
        if (!fp)
        {
            fprintf(stderr, "ERROR: Unable to open output file.\n");
            return EXITCODE_CANNOT_OPEN;
        }
    }
    else
    {
        fp = stdout;
    }

    fprintf(fp, "#pragma code_page(65001)\r\n\r\n");

    msg_tables_type::iterator it, end = g_msg_tables.end();
    for (it = g_msg_tables.begin(); it != end; ++it)
    {
        fprintf(fp, "LANGUAGE 0x%02X, 0x%02X\r\n",
            PRIMARYLANGID(it->first), SUBLANGID(it->first));

        MStringW wstr = it->second.Dump(pseudolocalize);
        MStringA str = MWideToAnsi(CP_UTF8, wstr.c_str()).c_str();

        fputs(str.c_str(), fp);
    }

    if (output_file)
        fclose(fp);

    if (ferror(fp))
    {
        if (output_file)
            DeleteFileW(output_file);
        fprintf(stderr, "ERROR: Unable to write output file.\n");
        return EXITCODE_CANNOT_OPEN;
    }

    return EXITCODE_SUCCESS;
}

int save_res(const wchar_t* output_file)
{
    MByteStreamEx bs;
    ResHeader header;
    if (!header.WriteTo(bs))
        return EXITCODE_INVALID_DATA;

    msg_tables_type::iterator it, end = g_msg_tables.end();
    for (it = g_msg_tables.begin(); it != end; ++it)
    {
        MByteStreamEx stream;
        it->second.SaveToStream(stream);

        header.DataSize = DWORD(stream.size());
        header.HeaderSize = header.GetHeaderSize(RT_MESSAGETABLE, 1);
        if (header.HeaderSize == 0 || header.HeaderSize >= 0x10000)
            return FALSE;

        header.type = RT_MESSAGETABLE;
        header.name = 1;
        header.DataVersion = 0;
        header.MemoryFlags = MEMORYFLAG_DISCARDABLE | MEMORYFLAG_PURE |
            MEMORYFLAG_MOVEABLE;
        header.LanguageId = it->first;
        header.Version = 0;
        header.Characteristics = 0;

        if (!header.WriteTo(bs))
            return FALSE;

        if (!bs.WriteData(&stream[0], stream.size()))
            return FALSE;

        bs.WriteDwordAlignment();
    }

    FILE* fp;
    if (output_file)
    {
        fp = _wfopen(output_file, L"wb");
        if (!fp)
        {
            fprintf(stderr, "ERROR: Unable to open output file.\n");
            return EXITCODE_CANNOT_OPEN;
        }
    }
    else
    {
        fp = stdout;
    }

    fwrite(&bs[0], bs.size(), 1, fp);

    if (output_file)
        fclose(fp);

    if (ferror(fp))
    {
        if (output_file)
            DeleteFileW(output_file);
        fprintf(stderr, "ERROR: Unable to write output file.\n");
        return EXITCODE_CANNOT_OPEN;
    }

    return EXITCODE_SUCCESS;
}


int save_bin(const wchar_t* output_file)
{
    MessageRes msg_res = g_msg_tables.begin()->second;

    MByteStreamEx stream;
    msg_res.SaveToStream(stream);

    FILE* fp;
    if (output_file)
    {
        fp = _wfopen(output_file, L"wb");
        if (!fp)
        {
            fprintf(stderr, "ERROR: Unable to open output file.\n");
            return EXITCODE_CANNOT_OPEN;
        }
    }
    else
    {
        fp = stdout;
    }

    fwrite(&stream[0], stream.size(), 1, fp);

    if (output_file)
        fclose(fp);

    if (ferror(fp))
    {
        if (output_file)
            DeleteFileW(output_file);
        fprintf(stderr, "ERROR: Unable to write output file.\n");
        return EXITCODE_CANNOT_OPEN;
    }

    return EXITCODE_SUCCESS;
}

bool IsUTF16File(const wchar_t* input_file)
{
    if (FILE* fp = _wfopen(input_file, L"rb"))
    {
        wchar_t ab[2];
        if (fread(ab, 1, 2, fp) == 2)
        {
            if (memcmp(ab, "\xFF\xFE", 2) == 0)
            {
                fclose(fp);
                return true;
            }
            if (ab[0] && !ab[1])
            {
                fclose(fp);
                return true;
            }
        }
        fclose(fp);
    }
    return false;
}

int load_rc(const wchar_t* input_file)
{
  

    // create a process
    
    std::string buffer;
    std::ifstream in(input_file);
    std::vector<std::string> fullfile;
    if (in) // 有该文件
    {
        while (std::getline(in, buffer)) // line中不包括每行的换行符
        {
            fullfile.push_back((buffer));
        }
    }
    in.close();
            // eat the output
            if (int ret = eat_output(fullfile))
                return ret;

            return EXITCODE_SUCCESS;

}

int load_bin(const wchar_t* input_file)
{
    FILE* fp = _wfopen(input_file, L"rb");
    if (!fp)
    {
        fprintf(stderr, "ERROR: Unable to open input file.\n");
        return EXITCODE_CANNOT_OPEN;
    }

    std::string contents;
    char buf[256];
    for (;;)
    {
        size_t len = fread(buf, 1, 256, fp);
        if (!len)
            break;
        contents.append(buf, len);
    }
    fclose(fp);

    MByteStreamEx stream(&contents[0], contents.size());
    if (!g_msg_tables[g_langid].LoadFromStream(stream, 1))
    {
        fprintf(stderr, "ERROR: Invalid data.\n");
        return EXITCODE_INVALID_DATA;
    }

    return EXITCODE_SUCCESS;
}

int load_res(const wchar_t* input_file)
{
    FILE* fp = _wfopen(input_file, L"rb");
    if (!fp)
    {
        fprintf(stderr, "ERROR: Unable to open input file.\n");
        return EXITCODE_CANNOT_OPEN;
    }

    std::wstring contents;
    wchar_t buf[256];
    for (;;)
    {
        size_t len = fread(buf, 1, 256, fp);
        if (!len)
            break;
        contents.append(buf, len);
    }
    fclose(fp);

    MByteStreamEx stream(&contents[0], contents.size());
    ResHeader header;
    while (header.ReadFrom(stream))
    {
        if (header.DataSize == 0)
        {
            stream.ReadDwordAlignment();
            continue;
        }

        if (header.DataSize > stream.remainder())
        {
            fprintf(stderr, "ERROR: Data is broken, invalid, or not supported.\n");
            return EXITCODE_INVALID_DATA;
        }

        MByteStreamEx bs(header.DataSize);
        if (!stream.ReadData(&bs[0], header.DataSize))
        {
            break;
        }
        if (!g_msg_tables[header.LanguageId].LoadFromStream(bs, 1))
        {
            fprintf(stderr, "ERROR: Data is broken, invalid, or not supported.\n");
            return EXITCODE_INVALID_DATA;
        }

        stream.ReadDwordAlignment();
    }

    return EXITCODE_SUCCESS;
}

int just_do_it(LPCWSTR inputfile,LPCWSTR outputfile,LPCWSTR impformat,LPCWSTR outformat,bool pseudolocalize=false)
{
    if (wcscmp(impformat,L"rc") == 0)
    {
        if (int ret = load_rc(inputfile))
            return ret;
    }
    else if (wcscmp(impformat, L"bin") == 0)
    {
        if (int ret = load_bin(inputfile))
            return ret;
    }

    if (wcscmp(outformat, L"rc") == 0)
    {
        return save_rc(outputfile,pseudolocalize);
    }
    else if (wcscmp(outformat, L"bin") == 0)
    {
        return save_bin(outputfile);
    }
    return 0;
}
const wchar_t* get_format(const wchar_t* file_path)
{
    const wchar_t* pch = mstrrchr(file_path, L'.');
    if (pch == NULL)
    {
        return L"rc";
    }
    else if (wcscmp(pch, L".rc") == 0)
    {
        return L"rc";
    }
    else if (wcscmp(pch, L".res") == 0)
    {
        return L"res";
    }
    else if (wcscmp(pch,L".bin") == 0)
    {
        return L"bin";
    }
        return L"rc";
    
}

//////////////////////////////////////////////////////////////////////////////

BOOL Pseudo_msgtbl(LPCWSTR input)
{

    std::wstring tmpfile = std::wstring(input)+L".msgtbl";
    bool pseudolocalize = true;
    int ret = just_do_it(input, tmpfile.c_str(), L"bin", L"rc", pseudolocalize);
    ret = just_do_it(tmpfile.c_str(),input, L"rc", L"bin", pseudolocalize);
    DeleteFileW(tmpfile.c_str());
    return ret==0;
}

//////////////////////////////////////////////////////////////////////////////