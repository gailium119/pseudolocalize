// pseudotest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include<fstream>
#include<Windows.h>
#include <string>
#include<vector>
#include <iostream>
#pragma warning(disable:4996)
#define random(x)(rand()%x)
namespace pseudo {
    std::wstring a[16] = { L"\xc2\xaa", L"\xc3\xa0", L"\xc3\xa1", L"\xc3\xa2", L"\xc3\xa3", L"\xc3\xa4", L"\xc3\xa5", L"\xc4\x83", L"\xc4\x81", L"\xc4\x85", L"\xc7\xbb", L"\xce\xac", L"\xce\xb1", L"\xd0\xb0", L"\xd0\xb4", L"\xe2\x88\x86" };
    std::wstring b[3] = { L"\xd0\xb2", L"\xd1\x8a", L"\xd1\x8c" };
    std::wstring c[8] = { L"\xc2\xa2", L"\xc3\xa7", L"\xc4\x87", L"\xc4\x89", L"\xc4\x8b", L"\xc4\x8d", L"\xcf\x82", L"\xd1\x81" };
    std::wstring d[4] = { L"\xc3\xb0", L"\xce\xb4", L"\xc4\x91", L"\xc4\x8f" };
    std::wstring e[17] = { L"\xc3\xa8", L"\xc3\xa9", L"\xc3\xaa", L"\xc3\xab", L"\xce\xad", L"\xce\xb5", L"\xd0\xb5", L"\xd0\xb7", L"\xd1\x8d", L"\xd1\x91", L"\xd1\x94", L"\xe2\x84\xae", L"\xc4\x95", L"\xc4\x93", L"\xc4\x97", L"\xc4\x99", L"\xc4\x9b" };
    std::wstring f[1] = { L"\xc6\x92" };
    std::wstring g[4] = { L"\xc4\x9f", L"\xc4\x9d", L"\xc4\xa1", L"\xc4\xa3" };
    std::wstring h[5] = { L"\xd0\xbd", L"\xd1\x92", L"\xd1\x9b", L"\xc4\xa7", L"\xc4\xa5" };
    std::wstring i[14] = { L"\xc3\xac", L"\xc3\xad", L"\xc3\xae", L"\xc3\xaf", L"\xce\x90", L"\xce\xaf", L"\xce\xb9", L"\xcf\x8a", L"\xd1\x96", L"\xd1\x97", L"\xc4\xab", L"\xc4\xa9", L"\xc4\xad", L"\xc4\xaf" };
    std::wstring j[2] = { L"\xd1\x98", L"\xc4\xb5" };
    std::wstring k[5] = { L"\xce\xba", L"\xd0\xba", L"\xd1\x9c", L"\xc4\xb7", L"\xc4\xb8" };
    std::wstring l[6] = { L"\xe2\x84\x93", L"\xc4\xbc", L"\xc4\xba", L"\xc4\xbe", L"\xc5\x80", L"\xc5\x82" };
    std::wstring m[3] = { L"\xd0\xbc", L"\xc9\xb1", L"\xe1\xb8\xbf" };
    std::wstring n[14] = { L"\xc3\xb1", L"\xce\xae", L"\xce\xb7", L"\xcf\x80", L"\xd0\xb8", L"\xd0\xb9", L"\xd0\xbb", L"\xd0\xbf", L"\xe2\x81\xbf", L"\xc5\x86", L"\xc5\x88", L"\xc5\x89", L"\xc5\x8b", L"\xc5\x84" };
    std::wstring o[17] = { L"\xc3\xb2", L"\xc3\xb3", L"\xc3\xb4", L"\xc3\xb5", L"\xc3\xb6", L"\xc3\xb8", L"\xc7\xbf", L"\xce\xa6", L"\xce\xb8", L"\xce\xbf", L"\xcf\x83", L"\xcf\x8c", L"\xd0\xb1", L"\xd0\xbe", L"\xc5\x8d", L"\xc5\x8f", L"\xc5\x91" };
    std::wstring p[3] = { L"\xcf\x81", L"\xcf\x86", L"\xd1\x80" };
    std::wstring q[1] = { L"\x71" };
    std::wstring r[7] = { L"\xd0\xb3", L"\xd1\x8f", L"\xd1\x93", L"\xd2\x91", L"\xc5\x99", L"\xc5\x95", L"\xc5\x97" };
    std::wstring s[5] = { L"\xd1\x95", L"\xc5\xa1", L"\xc5\x9b", L"\xc5\x9f", L"\xc5\x9d" };
    std::wstring t[5] = { L"\xcf\x84", L"\xd1\x82", L"\xc5\xa7", L"\xc5\xa3", L"\xc5\xa5" };
    std::wstring u[18] = { L"\xc2\xb5", L"\xc3\xb9", L"\xc3\xba", L"\xc3\xbb", L"\xc3\xbc", L"\xce\xb0", L"\xce\xbc", L"\xcf\x85", L"\xcf\x8b", L"\xcf\x8d", L"\xd1\x86", L"\xd1\x9f", L"\xc5\xb3", L"\xc5\xa9", L"\xc5\xb1", L"\xc5\xaf", L"\xc5\xad", L"\xc5\xab" };
    std::wstring v[2] = { L"\xce\xbd", L"\xca\x8b" };
    std::wstring w[8] = { L"\xcf\x89", L"\xcf\x8e", L"\xd1\x88", L"\xd1\x89", L"\xe1\xba\x85", L"\xe1\xba\x81", L"\xe1\xba\x83", L"\xc5\xb5" };
    std::wstring x[3] = { L"\xc3\x97", L"\xd0\xb6", L"\xd1\x85" };
    std::wstring y[7] = { L"\xc3\xbd", L"\xc3\xbf", L"\xce\xb3", L"\xd1\x83", L"\xd1\x9e", L"\xe1\xbb\xb3", L"\xc5\xb7" };
    std::wstring z[3] = { L"\xc5\xbe", L"\xc5\xba", L"\xc5\xbc" };
    std::wstring A[16] = { L"\xc3\x80", L"\xc3\x81", L"\xc3\x82", L"\xc3\x83", L"\xc3\x84", L"\xc3\x85", L"\xc4\x80", L"\xc4\x82", L"\xc4\x84", L"\xc7\xba", L"\xce\x86", L"\xce\x91", L"\xce\x94", L"\xce\x9b", L"\xd0\x90", L"\xd0\x94" };
    std::wstring B[7] = { L"\xc3\x9f", L"\xce\x92", L"\xce\xb2", L"\xd0\x91", L"\xd0\x92", L"\xd0\xaa", L"\xd0\xac" };
    std::wstring C[7] = { L"\xc3\x87", L"\xc4\x86", L"\xc4\x88", L"\xc4\x8a", L"\xc4\x8c", L"\xd0\xa1", L"\xe2\x82\xac" };
    std::wstring D[3] = { L"\xc3\x90", L"\xc4\x8e", L"\xc4\x90" };
    std::wstring E[18] = { L"\xc3\x88", L"\xc3\x89", L"\xc3\x8a", L"\xc3\x8b", L"\xce\x88", L"\xce\x95", L"\xce\xa3", L"\xd0\x81", L"\xd0\x84", L"\xd0\x95", L"\xd0\x97", L"\xd0\xad", L"\xe2\x88\x91", L"\xc4\x92", L"\xc4\x96", L"\xc4\x98", L"\xc4\x9a", L"\xc4\x94" };
    std::wstring F[1] = { L"\xe2\x82\xa3" };
    std::wstring G[4] = { L"\xc4\x9c", L"\xc4\xa0", L"\xc4\xa2", L"\xc4\x9e" };
    std::wstring H[5] = { L"\xce\x89", L"\xce\x97", L"\xd0\x9d", L"\xc4\xa4", L"\xc4\xa6" };
    std::wstring I[12] = { L"\xc3\x8c", L"\xc3\x8d", L"\xc3\x8e", L"\xce\x8a", L"\xce\xaa", L"\xd0\x86", L"\xd0\x87", L"\xc4\xa8", L"\xc4\xaa", L"\xc4\xac", L"\xc4\xae", L"\xc4\xb0" };
    std::wstring J[2] = { L"\xd0\x88", L"\xc4\xb4" };
    std::wstring K[4] = { L"\xce\x9a", L"\xd0\x8c", L"\xd0\x9a", L"\xc4\xb6" };
    std::wstring L[7] = { L"\xc2\xa3", L"\xe2\x82\xa4", L"\xc4\xb9", L"\xc4\xbb", L"\xc4\xbd", L"\xc4\xbf", L"\xc5\x81" };
    std::wstring M[2] = { L"\xce\x9c", L"\xd0\x9c" };
    std::wstring N[11] = { L"\xc3\x91", L"\xce\x9d", L"\xd0\x98", L"\xd0\x99", L"\xd0\x9b", L"\xd0\x9f", L"\xe2\x88\x8f", L"\xc5\x83", L"\xc5\x87", L"\xc5\x85", L"\xc5\x8a" };
    std::wstring O[18] = { L"\xc3\x92", L"\xc3\x93", L"\xc3\x94", L"\xc3\x95", L"\xc3\x96", L"\xc3\x98", L"\xc7\xbe", L"\xce\x8c", L"\xce\x8f", L"\xce\x98", L"\xce\x9f", L"\xce\xa9", L"\xd0\x9e", L"\xd0\xa4", L"\xe2\x84\xa6", L"\xc5\x8c", L"\xc5\x8e", L"\xc5\x90" };
    std::wstring P[3] = { L"\xc3\x9e", L"\xce\xa1", L"\xd0\xa0" };
    std::wstring Q[1] = { L"\x51" };
    std::wstring R[8] = { L"\xce\x93", L"\xd0\x83", L"\xd0\x93", L"\xd0\xaf", L"\xd2\x90", L"\xc5\x94", L"\xc5\x96", L"\xc5\x98" };
    std::wstring S[5] = { L"\xd0\x85", L"\xc5\x9a", L"\xc5\x9c", L"\xc5\x9e", L"\xc5\xa0" };
    std::wstring T[5] = { L"\xce\xa4", L"\xd0\xa2", L"\xc5\xa2", L"\xc5\xa4", L"\xc5\xa6" };
    std::wstring U[12] = { L"\xc3\x99", L"\xc3\x9a", L"\xc3\x9b", L"\xc3\x9c", L"\xd0\x8f", L"\xd0\xa6", L"\xc5\xa8", L"\xc5\xaa", L"\xc5\xac", L"\xc5\xae", L"\xc5\xb0", L"\xc5\xb2" };
    std::wstring V[1] = { L"\x56" };
    std::wstring W[5] = { L"\xd0\xa8", L"\xe1\xba\x80", L"\xe1\xba\x82", L"\xe1\xba\x84", L"\xc5\xb4" };
    std::wstring X[4] = { L"\xce\xa7", L"\xcf\x87", L"\xd0\x96", L"\xd0\xa5" };
    std::wstring Y[8] = { L"\xc2\xa5", L"\xc3\x9d", L"\xce\x8e", L"\xce\xa5", L"\xce\xab", L"\xd0\x8e", L"\xe1\xbb\xb2", L"\xc5\xb6" };
    std::wstring Z[4] = { L"\xce\x96", L"\xc5\xb9", L"\xc5\xbb", L"\xc5\xbd" };
    std::wstring charset = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
}
using namespace pseudo;
std::wstring Pseudo_localize(std::wstring str,bool genid=true, bool wraparound=true,bool extend=true) {
    std::wstring after;
    wchar_t buffer[5] = L"";
    for (int cnt = 0; cnt <= 4; cnt++) {

        buffer[cnt] = charset[random(62)];
    }
    std::wstring buf = std::wstring(buffer, 5);
    int len = str.length();
    bool nolower = true;
    for (int cnt = 0; cnt < len; cnt++) {
        std::wstring out;
        switch (str[cnt]) {
        case 'a': out = a[random(16)]; nolower = false; break;
        case 'b': out = b[random(3)];  nolower = false; break;
        case 'c': out = c[random(8)]; nolower = false; break;
        case 'd': out = d[random(4)];  nolower = false; break;
        case 'e': out = e[random(17)]; nolower = false; break;
        case 'f': out = f[random(1)]; nolower = false; break;
        case 'g': out = g[random(4)]; nolower = false; break;
        case 'h': out = h[random(5)]; nolower = false; break;
        case 'i': out = i[random(14)]; nolower = false; break;
        case 'j': out = j[random(2)];  nolower = false; break;
        case 'k': out = k[random(5)]; nolower = false; break;
        case 'l': out = l[random(6)]; nolower = false; break;
        case 'm': out = m[random(3)];  nolower = false; break;
        case 'n': out = n[random(14)]; nolower = false; break;
        case 'o': out = o[random(17)]; nolower = false; break;
        case 'p': out = p[random(3)]; nolower = false; break;
        case 'q': out = q[random(1)]; nolower = false; break;
        case 'r': out = r[random(7)]; nolower = false; break;
        case 's': out = s[random(5)]; nolower = false; break;
        case 't': out = t[random(5)]; nolower = false; break;
        case 'u': out = u[random(18)]; nolower = false; break;
        case 'v': out = v[random(2)]; nolower = false; break;
        case 'w': out = w[random(8)]; nolower = false; break;
        case 'x': out = x[random(3)]; nolower = false; break;
        case 'y': out = y[random(7)];  nolower = false; break;
        case 'z': out = z[random(3)];  nolower = false; break;
        case 'A': out = A[random(16)]; break;
        case 'B': out = B[random(7)]; break;
        case 'C': out = C[random(7)]; break;
        case 'D': out = D[random(3)]; break;
        case 'E': out = E[random(18)]; break;
        case 'F': out = F[random(1)]; break;
        case 'G': out = G[random(4)]; break;
        case 'H': out = H[random(5)]; break;
        case 'I': out = I[random(12)]; break;
        case 'J': out = J[random(2)]; break;
        case 'K': out = K[random(4)]; break;
        case 'L': out = L[random(7)]; break;
        case 'M': out = M[random(2)]; break;
        case 'N': out = N[random(11)]; break;
        case 'O': out = O[random(18)]; break;
        case 'P': out = P[random(3)]; break;
        case 'Q': out = Q[random(1)]; break;
        case 'R': out = R[random(8)]; break;
        case 'S': out = S[random(5)]; break;
        case 'T': out = T[random(5)]; break;
        case 'U': out = U[random(12)]; break;
        case 'V': out = V[random(1)]; break;
        case 'W': out = W[random(5)]; break;
        case 'X': out = X[random(4)]; break;
        case 'Y': out = Y[random(8)]; break;
        case 'Z': out = Z[random(4)]; break;
        case '\n':out = std::wstring(L"\n") + L"[" + buf + L"]"; break;
        default: out = str[cnt]; break;
        }
        after += out;
    }
    wprintf(L"%ls\n", after.c_str());
    if (nolower) {
        // supposingly a macro
        return str;
    }
    //wrap
    if(extend){
    int lencnt = (int)floor(len * 3.0 / 10.0 + 0.5);
    for (int cnt = 1; cnt <= lencnt; cnt++) {
        if (cnt % 4 == 1) after += L" ";
        else after += L"!";
    }
    }
    buf = std::wstring(L"[") + buf + L"]";
    if(wraparound) after= std::wstring(L"[") + after + L"]";
    if (genid)after = buf + after;
    return after;
}
void ReadData1(std::wstring path) {
    std::wstring buffer;
    std::wifstream in(path);
    int cnt = 1;
    if (in) // 有该文件
    {
        while (std::getline(in, buffer)) // line中不包括每行的换行符
        {
            wprintf(L"std::wstring %c", buffer[0]);
            buffer = buffer.substr(buffer.find(L',') + 1);
            //std::wcout << buffer << std::endl; 
            int len = buffer.length();
            for (int i = 0; i < len; i++) {
                if (buffer[i] == L',') { cnt += 1; }
            }
            wprintf(L"[%d] = {L\"", cnt);
            for (int i = 0; i < len; i++) {
                if (buffer[i] == L',') { std::wcout << L"\", L\""; }
                // else wprintf(L"\\x%x", buffer[i]);
                else std::wcout<<buffer[i];
            }
            cnt = 1;
            std::wcout << L"\"};" << std::endl;
        }
    }
    in.close();
}
void ReadData2(std::wstring path) {
    std::wstring buffer;
    std::wifstream in(path);
    int cnt = 1;
    if (in) // 有该文件
    {
        while (std::getline(in, buffer)) // line中不包括每行的换行符
        {
            wprintf(L"case \'%c\' : out = %c[random(", buffer[0], buffer[0]);
            buffer = buffer.substr(buffer.find(L',') + 1);
            //std::wcout << buffer << std::endl; 
            int len = buffer.length();
            for (int i = 0; i < len; i++) {
                if (buffer[i] == L',') { cnt += 1; }
            }
            wprintf(L"%d)]; break;", cnt);
            cnt = 1;
            std::wcout << std::endl;
        }
    }
    in.close();
}
void Pseudo_file(std::wstring path) {
    std::wstring buffer;
    std::wifstream in(path);
    std::vector<std::wstring> fullfile;
    if (in) // 有该文件
    {
        while (std::getline(in, buffer)) // line中不包括每行的换行符
        {
            fullfile.push_back(Pseudo_localize(buffer));
        }
    }
    in.close();
    std::wofstream out(path);
    for (int cnt = 0; cnt < fullfile.size(); cnt++) {
        out << fullfile[cnt] << std::endl;
    }
    out.close();
}
