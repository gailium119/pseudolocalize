// pseudolocalize.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include"librc.h"
#include <windows.h>
#include "pseudo.h"
#include <time.h>
#include "xml.h"
#include "prires.h"
int wmain(int argc, wchar_t** argv)
{
    SetConsoleCP(65001)
        ;
    SetConsoleOutputCP(65001);
    srand((int)time(0));
    if (argc < 2) {
        wprintf(L"pseudotest.exe <Command> [Options]\n");
        wprintf(L"Command Options:\n");
        wprintf(L"pseudotest.exe string [string]\n");
        wprintf(L"pseudotest.exe file [filename]\n");
        wprintf(L"pseudotest.exe rc [inputmui] [outputrc]\n");
        wprintf(L"pseudotest.exe xml [filename]\n");
        wprintf(L"pseudotest.exe adml [filename]\n");
        wprintf(L"pseudotest.exe infloc [filename]\n");
        wprintf(L"pseudotest.exe ini [filename]\n");
        wprintf(L"pseudotest.exe mfl [filename]\n");
        wprintf(L"pseudotest.exe pri [filename]\n");
        wprintf(L"pseudotest.exe psd1 [filename]\n");
        return 0;
    }
    if (_wcsicmp(argv[1], L"string") == 0) {
        if (argc != 3) {
            wprintf(L"Invalid Usage\npseudotest.exe string [WhatEver]");
        }
        wprintf(L"%ls\n", Pseudo_localize(argv[2]).c_str());
    }
    else if (_wcsicmp(argv[1], L"file") == 0) Pseudo_file(argv[2]);
    else if (_wcsicmp(argv[1], L"rc") == 0) {
        if (argc != 4) {
            wprintf(L"Invalid Usage\npseudotest.exe rc [inputmui] [outputrc]");
        }
        ExportRC(argv[2],argv[3],true);
    }
    else if (_wcsicmp(argv[1], L"xml") == 0) {
        CoInitialize(0);
        std::vector<std::wstring> textxpaths;
        std::vector<std::pair<std::wstring, std::wstring>>attrxpaths;
        textxpaths.push_back(L"//String");
        MyXmlDoc doc;
        doc.Load(argv[2]);
        MyXmlElementTable table;
        table.SelectFromDocument(doc, L"//_locTag");
        auto elements = table.GetElements();
        for (int cnt = 0; cnt < elements.size(); cnt++) {
            std::wstring keyname = std::wstring(L"//") + elements[cnt].GetText();
            std::wstring attrname = elements[cnt].GetAttr(L"_locAttrData");
            attrxpaths.push_back(std::make_pair(keyname, attrname));
            elements[cnt].Release();
        }

        table.Release();
        doc.Release();
        Pseudo_xml(argv[2], textxpaths, attrxpaths,false);
    }
    else if (_wcsicmp(argv[1], L"adml") == 0) {
        CoInitialize(0);
        std::vector<std::wstring> textxpaths;
        std::vector<std::pair<std::wstring, std::wstring>>attrxpaths;
        textxpaths.push_back(L"//string");
        textxpaths.push_back(L"//dropdownList");
        textxpaths.push_back(L"//checkBox");
        textxpaths.push_back(L"//listBox");
        textxpaths.push_back(L"//label");
        textxpaths.push_back(L"//decimalTextBox");
        textxpaths.push_back(L"//displayName");
        textxpaths.push_back(L"//description");
        textxpaths.push_back(L"//multiTextBox");
        textxpaths.push_back(L"//text");
        Pseudo_xml(argv[2], textxpaths, attrxpaths, false);
    }
    else if (_wcsicmp(argv[1], L"infloc") == 0) Pseudo_inf(argv[2]);
    else if (_wcsicmp(argv[1], L"ini") == 0) Pseudo_ini(argv[2]);
    else if (_wcsicmp(argv[1], L"mfl") == 0) Pseudo_mfl(argv[2]);
    else if (_wcsicmp(argv[1], L"pri") == 0) {
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        CreatePri(argv[2],true);
    }
    else if (_wcsicmp(argv[1], L"psd1") == 0) Pseudo_psd1(argv[2]);

    // ReadData1(L"data.txt");
    // ReadData2(L"data.txt");
    return 0;
}



// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
