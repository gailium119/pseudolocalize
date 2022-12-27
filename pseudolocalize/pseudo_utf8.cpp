#include <string>
#include"xml.h"
#include<fstream>
#include <codecvt>
#include "MString.hpp"
#include "pseudo.h"
namespace pseudo_utf8
{
	std::wstring a[16] = { L"ª", L"à", L"á", L"â", L"ã", L"ä", L"å", L"ă", L"ā", L"ą", L"ǻ", L"ά", L"α", L"а", L"д", L"∆" };
	std::wstring b[3] = { L"в", L"ъ", L"ь" };
	std::wstring c[8] = { L"¢", L"ç", L"ć", L"ĉ", L"ċ", L"č", L"ς", L"с" };
	std::wstring d[4] = { L"ð", L"δ", L"đ", L"ď" };
	std::wstring e[17] = { L"è", L"é", L"ê", L"ë", L"έ", L"ε", L"е", L"з", L"э", L"ё", L"є", L"℮", L"ĕ", L"ē", L"ė", L"ę", L"ě" };
	std::wstring f[1] = { L"ƒ" };
	std::wstring g[4] = { L"ğ", L"ĝ", L"ġ", L"ģ" };
	std::wstring h[5] = { L"н", L"ђ", L"ћ", L"ħ", L"ĥ" };
	std::wstring i[14] = { L"ì", L"í", L"î", L"ï", L"ΐ", L"ί", L"ι", L"ϊ", L"і", L"ї", L"ī", L"ĩ", L"ĭ", L"į" };
	std::wstring j[2] = { L"ј", L"ĵ" };
	std::wstring k[5] = { L"κ", L"к", L"ќ", L"ķ", L"ĸ" };
	std::wstring l[6] = { L"ℓ", L"ļ", L"ĺ", L"ľ", L"ŀ", L"ł" };
	std::wstring m[3] = { L"м", L"ɱ", L"ḿ" };
	std::wstring n[14] = { L"ñ", L"ή", L"η", L"π", L"и", L"й", L"л", L"п", L"ⁿ", L"ņ", L"ň", L"ŉ", L"ŋ", L"ń" };
	std::wstring o[17] = { L"ò", L"ó", L"ô", L"õ", L"ö", L"ø", L"ǿ", L"Φ", L"θ", L"ο", L"σ", L"ό", L"б", L"о", L"ō", L"ŏ", L"ő" };
	std::wstring p[3] = { L"ρ", L"φ", L"р" };
	std::wstring q[1] = { L"q" };
	std::wstring r[7] = { L"г", L"я", L"ѓ", L"ґ", L"ř", L"ŕ", L"ŗ" };
	std::wstring s[5] = { L"ѕ", L"š", L"ś", L"ş", L"ŝ" };
	std::wstring t[5] = { L"τ", L"т", L"ŧ", L"ţ", L"ť" };
	std::wstring u[18] = { L"µ", L"ù", L"ú", L"û", L"ü", L"ΰ", L"μ", L"υ", L"ϋ", L"ύ", L"ц", L"џ", L"ų", L"ũ", L"ű", L"ů", L"ŭ", L"ū" };
	std::wstring v[2] = { L"ν", L"ʋ" };
	std::wstring w[8] = { L"ω", L"ώ", L"ш", L"щ", L"ẅ", L"ẁ", L"ẃ", L"ŵ" };
	std::wstring x[3] = { L"×", L"ж", L"х" };
	std::wstring y[7] = { L"ý", L"ÿ", L"γ", L"у", L"ў", L"ỳ", L"ŷ" };
	std::wstring z[3] = { L"ž", L"ź", L"ż" };
	std::wstring A[16] = { L"À", L"Á", L"Â", L"Ã", L"Ä", L"Å", L"Ā", L"Ă", L"Ą", L"Ǻ", L"Ά", L"Α", L"Δ", L"Λ", L"А", L"Д" };
	std::wstring B[7] = { L"ß", L"Β", L"β", L"Б", L"В", L"Ъ", L"Ь" };
	std::wstring C[7] = { L"Ç", L"Ć", L"Ĉ", L"Ċ", L"Č", L"С", L"€" };
	std::wstring D[3] = { L"Ð", L"Ď", L"Đ" };
	std::wstring E[18] = { L"È", L"É", L"Ê", L"Ë", L"Έ", L"Ε", L"Σ", L"Ё", L"Є", L"Е", L"З", L"Э", L"∑", L"Ē", L"Ė", L"Ę", L"Ě", L"Ĕ" };
	std::wstring F[1] = { L"₣" };
	std::wstring G[4] = { L"Ĝ", L"Ġ", L"Ģ", L"Ğ" };
	std::wstring H[5] = { L"Ή", L"Η", L"Н", L"Ĥ", L"Ħ" };
	std::wstring I[12] = { L"Ì", L"Í", L"Î", L"Ί", L"Ϊ", L"І", L"Ї", L"Ĩ", L"Ī", L"Ĭ", L"Į", L"İ" };
	std::wstring J[2] = { L"Ј", L"Ĵ" };
	std::wstring K[4] = { L"Κ", L"Ќ", L"К", L"Ķ" };
	std::wstring L[7] = { L"£", L"₤", L"Ĺ", L"Ļ", L"Ľ", L"Ŀ", L"Ł" };
	std::wstring M[2] = { L"Μ", L"М" };
	std::wstring N[11] = { L"Ñ", L"Ν", L"И", L"Й", L"Л", L"П", L"∏", L"Ń", L"Ň", L"Ņ", L"Ŋ" };
	std::wstring O[18] = { L"Ò", L"Ó", L"Ô", L"Õ", L"Ö", L"Ø", L"Ǿ", L"Ό", L"Ώ", L"Θ", L"Ο", L"Ω", L"О", L"Ф", L"Ω", L"Ō", L"Ŏ", L"Ő" };
	std::wstring P[3] = { L"Þ", L"Ρ", L"Р" };
	std::wstring Q[1] = { L"Q" };
	std::wstring R[8] = { L"Γ", L"Ѓ", L"Г", L"Я", L"Ґ", L"Ŕ", L"Ŗ", L"Ř" };
	std::wstring S[5] = { L"Ѕ", L"Ś", L"Ŝ", L"Ş", L"Š" };
	std::wstring T[5] = { L"Τ", L"Т", L"Ţ", L"Ť", L"Ŧ" };
	std::wstring U[12] = { L"Ù", L"Ú", L"Û", L"Ü", L"Џ", L"Ц", L"Ũ", L"Ū", L"Ŭ", L"Ů", L"Ű", L"Ų" };
	std::wstring V[1] = { L"V" };
	std::wstring W[5] = { L"Ш", L"Ẁ", L"Ẃ", L"Ẅ", L"Ŵ" };
	std::wstring X[4] = { L"Χ", L"χ", L"Ж", L"Х" };
	std::wstring Y[8] = { L"¥", L"Ý", L"Ύ", L"Υ", L"Ϋ", L"Ў", L"Ỳ", L"Ŷ" };                                    
    std::wstring Z[4] = { L"Ζ", L"Ź", L"Ż", L"Ž" };
	std::wstring charset = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

}
#define random(x)(rand()%x)
std::wstring& replace_all(std::wstring& src, const std::wstring& old_value, const std::wstring& new_value) {
    // 每次重新定位起始位置，防止上轮替换后的字符串形成新的old_value
    for (std::wstring::size_type pos(0); pos != std::wstring::npos; pos += new_value.length()) {
        if ((pos = src.find(old_value, pos)) != std::wstring::npos) {
            src.replace(pos, old_value.length(), new_value);
        }
        else break;
    }
    return src;
}
using namespace pseudo_utf8;
std::wstring Pseudo_localize_utf8(std::wstring str, bool genid, bool wraparound, bool extend,bool genidwithnewline, bool skipempty) {
    std::wstring after;
    wchar_t buffer[5] = L"";
    for (int cnt = 0; cnt <= 4; cnt++) {

        buffer[cnt] = charset[random(62)];
    }
    if (str.length() > 10 && str[0] == '<' && str[1] == '?' && str[2] == 'x' && str[3] == 'm' && str[4] == 'l') {
        // an xml file
        return str;
    }
    std::wstring buf = std::wstring(buffer, 5);
    size_t len = str.length();
    bool nolower = true;
    bool hasunderline = false;
    for (size_t cnt = 0; cnt < len; cnt++) {
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
        case '\n': {
            if (genid && genidwithnewline) out = std::wstring(L"\n") + L"[" + buf + L"]";
            else out = L"\n"; 
            break;
        }
        case '%': {
            for (; cnt < len; cnt++) {
                out += str[cnt];
                if (str[cnt] >= 'A' && str[cnt] <= 'z' && str[cnt]!='l'&&str[cnt]!='W') {
                    break;
                }
                if (str[cnt] == 'W') {
                    for (cnt++; cnt < len; cnt++) {
                        out += str[cnt];
                        if (str[cnt] == '%') break;
                    }
                    break;
                }
            }
            break;
        }
        case '<': {
            for (; cnt < len; cnt++) {
                out += str[cnt];
                if (str[cnt] == '>') {
                    break;
                }
            }
            break;
        }
        case '&': {
            for (; cnt < len; cnt++) {
                out += str[cnt];
                if (str[cnt] >= 'A' && str[cnt] <= 'z') {
                    break;
                }
            }
            break;
        }
        case '_':hasunderline = true; out = str[cnt]; break;
        default: out = str[cnt]; break;
        }
        after += out;
    }
    bool isempty = true;
    for (int cnt = 0; cnt < after.length(); cnt++) {
        if (after[cnt]!=' '&& after[cnt]!='\t' && after[cnt] != '\0' && after[cnt] != '\n' && after[cnt] != '\r') {
            isempty = false;
        }
    }
   isempty= isempty || (after.length() == 0);
   if (isempty && skipempty)return str;
    if (nolower&&hasunderline) {
        // supposingly a macro
        return str;
    }
    //wrap
    if (extend) {
        int lencnt = (int)floor(len * 3.0 / 10.0 + 0.5);
        for (int cnt = 1; cnt <= lencnt; cnt++) {
            if (cnt % 4 == 1) after += L" ";
            else after += L"!";
        }
    }
    buf = std::wstring(L"[") + buf + L"]";
    if (wraparound) after = std::wstring(L"[") + after + L"]";
    if (genid)after = buf + after;
    return after;
}
std::wstring Pseudo_localize_utf8_xml(std::wstring str, bool genid, bool wraparound, bool extend, bool genidwithnewline, bool skipempty) {
    //Process &lt
    replace_all(str,L"&lt;", L"<");
    replace_all(str, L"&gt;", L">");

    std::wstring after;
    wchar_t buffer[5] = L"";
    for (int cnt = 0; cnt <= 4; cnt++) {

        buffer[cnt] = charset[random(62)];
    }
    if (str.length() > 10 && str[0] == '<' && str[1] == '?' && str[2] == 'x' && str[3] == 'm' && str[4] == 'l') {
        // an xml file
        return str;
    }
    std::wstring buf = std::wstring(buffer, 5);
    size_t len = str.length();
    bool nolower = true;
    bool hasunderline = false;
    int reallen = 0;
    for (int cnt = 0; cnt < len; cnt++) {
        std::wstring out;
        ++reallen;
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
        case '\n': {
            if (genid && genidwithnewline) out = std::wstring(L"\n") + L"[" + buf + L"]";
            else out = L"\n";
            break;
        }
        case '%': {
            for (; cnt < len; cnt++) {
                out += str[cnt];
                if (str[cnt] >= 'A' && str[cnt] <= 'z' && str[cnt] != 'l' && str[cnt] != 'W') {
                    break;
                }
                if (str[cnt] == 'W') {
                    for (cnt++; cnt < len; cnt++) {
                        out += str[cnt];
                        if (str[cnt] == '%') break;
                    }
                    break;
                }
            }
            break;
        }
        case '<': {
            for (; cnt < len; cnt++) {
                out += str[cnt];
                if (str[cnt] == '>') {
                    break;
                }
            }
            break;
        }
        case '&': {
            for (; cnt < len; cnt++) {
                out += str[cnt];
                if (str[cnt] >= 'A' && str[cnt] <= 'z') {
                    break;
                }
            }
            break;
        }
        case '_':hasunderline = true; out = str[cnt]; break;
        default: out = str[cnt]; break;
        }
        after += out;
    }
    bool isempty = true;
    for (int cnt = 0; cnt < after.length(); cnt++) {
        if (after[cnt] != ' ' && after[cnt] != '\t' && after[cnt] != '\0' && after[cnt] != '\n' && after[cnt] != '\r' && after[cnt] != ' ') {
            isempty = false;
        }
    }
    isempty = isempty || (after.length() == 0);
    if (isempty && skipempty)return str;
    if (nolower && hasunderline) {
        // supposingly a macro
        return str;
    }
    //wrap
    if (extend) {
        int lencnt = (int)floor(reallen * 3.0 / 10.0 + 0.5);
        for (int cnt = 1; cnt <= lencnt; cnt++) {
            if (cnt % 4 == 1) after += L" ";
            else after += L"!";
        }
    }
    buf = std::wstring(L"[") + buf + L"]";
    if (wraparound) after = std::wstring(L"[") + after + L"]";
    if (genid)after = buf + after;
    return after;
}

void Pseudo_xml(LPCWSTR xmlpath,std::vector<std::wstring> textxpaths,std::vector<std::pair<std::wstring,std::wstring>>attrxpaths,bool dirty) {
    MyXmlDoc doc;
    doc.Load(xmlpath);
    for (int cnt = 0; cnt < textxpaths.size(); cnt++)
    {
        MyXmlElementTable table;
        table.SelectFromDocument(doc, textxpaths[cnt].c_str());
        auto elements = table.GetElements();
        for (int cnt2 = 0; cnt2 < elements.size(); cnt2 ++ ) {
            std::wstring str; 
            if(dirty)str= elements[cnt2].GetTextDirty();
            else str = elements[cnt2].GetText();
            if(!str.empty())elements[cnt2].SetText(Pseudo_localize_utf8_xml(str, true, true, true, !dirty, true));
            elements[cnt2].Release();
        }
        table.Release();
    }
    doc.Save(xmlpath);
    doc.Release();
    if (dirty) {
        std::wstring buffer;
        std::wifstream in(xmlpath);
        std::vector<std::wstring> fullfile;
        if (in) // 有该文件
        {
            while (std::getline(in, buffer)) // line中不包括每行的换行符
            {
                replace_all(buffer, L"&lt;", L"<");
                replace_all(buffer, L"&gt;", L">");

                fullfile.push_back((buffer));
            }
        }
        in.close();
        std::wofstream out(xmlpath);
        for (int cnt = 0; cnt < fullfile.size(); cnt++) {
            out << fullfile[cnt] << std::endl;
        }
        out.close();
    }
    doc.Load(xmlpath);
    for (int cnt = 0; cnt < attrxpaths.size(); cnt++)
    {
        MyXmlElementTable table;
        table.SelectFromDocument(doc, attrxpaths[cnt].first.c_str());
        auto elements = table.GetElements();
        for (int cnt2 = 0; cnt2 < elements.size(); cnt2++) {
          if(_wcsicmp(elements[cnt2].GetAttr(L"type").c_str(), L"string") == 0|| _wcsicmp(attrxpaths[cnt].second.c_str(), L"value") != 0) elements[cnt2].SetAttr(attrxpaths[cnt].second, Pseudo_localize_utf8(elements[cnt2].GetAttr(attrxpaths[cnt].second), true, true, true, false, true));
            elements[cnt2].Release();
        }
        table.Release();
    }
    doc.Save(xmlpath);
    doc.Release();
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

void Pseudo_inf_utf16(std::wstring path) {
    std::u16string buffer;
    std::ifstream fin(path, std::ios::binary);
    fin.seekg(0, std::ios::end);
    size_t size = (size_t)fin.tellg();

    //skip BOM
    fin.seekg(2, std::ios::beg);
    size -= 2;

    std::u16string u16((size / 2) + 1, '\0');
    fin.read((char*)&u16[0], size);

    std::string utf8 = std::wstring_convert<
        std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(u16);
    std::wstring widestring = MAnsiToWide(65001, utf8).c_str();
    utf8.clear();
    std::vector<std::wstring> fullfile;
    mstr_split(fullfile, widestring, L"\n");
    widestring.clear();    fin.close();

    FILE* fp;
    _wfopen_s(&fp, path.c_str() , L"w, ccs=UTF-16LE");
    if (fp) {
        bool pseudolocalize = false;
        for (int cnt = 0; cnt < fullfile.size(); cnt++) {
            replace_all(fullfile[cnt], L"\r", L"");
            if (_wcsicmp(fullfile[cnt].c_str(), L"[Strings.0409]") == 0)fullfile[cnt] = L"[Strings.0501]";
            if (!fullfile[cnt].empty()&&fullfile[cnt][0] == ';') {
                if (fullfile[cnt].find(L"ocalizable") != std::wstring::npos) {
                    if (fullfile[cnt].find(L"on") == std::wstring::npos) pseudolocalize = true;
                    else pseudolocalize = false;
                }
            }
            else if (!fullfile[cnt].empty() && pseudolocalize) {
                std::vector<std::wstring> strings;
                mstr_split(strings, fullfile[cnt], L"\"");
                size_t size = strings.size();
                fullfile[cnt].clear();
                for (size_t cnt2 = 0; cnt2 < size; cnt2++) {
                    if (cnt2 % 2 == 1) fullfile[cnt] += Pseudo_localize_utf8(strings[cnt2]);
                    else fullfile[cnt] += strings[cnt2];
                    if (cnt2 != size - 1) fullfile[cnt] += L"\"";
                }
            }
            fwprintf_s(fp, L"%ls\n", fullfile[cnt].c_str());
        }
        fclose(fp);
    }
    return;

}
void Pseudo_ini_utf16(std::wstring path) {
    std::u16string buffer;
    std::ifstream fin(path, std::ios::binary);
    fin.seekg(0, std::ios::end);
    size_t size = (size_t)fin.tellg();

    //skip BOM
    fin.seekg(2, std::ios::beg);
    size -= 2;

    std::u16string u16((size / 2) + 1, '\0');
    fin.read((char*)&u16[0], size);

    std::string utf8 = std::wstring_convert<
        std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(u16);
    std::wstring widestring = MAnsiToWide(65001, utf8).c_str();
    utf8.clear();
    std::vector<std::wstring> fullfile;
    mstr_split(fullfile, widestring, L"\n");
    widestring.clear();
    fin.close();
    FILE* fp;
    _wfopen_s(&fp, path.c_str(), L"w, ccs=UTF-16LE");
    if (fp) {
        bool pseudolocalize = false;
        for (int cnt = 0; cnt < fullfile.size(); cnt++) {
            replace_all(fullfile[cnt], L"\r", L"");
            if (fullfile[cnt].find(L"009=") != std::wstring::npos&& !pseudolocalize) fullfile[cnt] = L"101=UNASSIGNED LANGUAGE ID";
            if (!fullfile[cnt].empty() && fullfile[cnt][0] == '[') {
                if (fullfile[cnt].find(L"bjects") != std::wstring::npos|| fullfile[cnt].find(L"ext") != std::wstring::npos) {
                    pseudolocalize = true;
                  
                }  else pseudolocalize = false;
            }
            else if (!fullfile[cnt].empty() && pseudolocalize) {
                replace_all(fullfile[cnt], L"009", L"101");
                std::vector<std::wstring> strings;
                mstr_split(strings, fullfile[cnt], L"=");
                size_t size = strings.size();
                fullfile[cnt].clear();
                for (size_t cnt2 = 0; cnt2 < size; cnt2++) {
                    if (cnt2 % 2 == 1&&fullfile[cnt].find(L"_000_") == std::wstring::npos) fullfile[cnt] += Pseudo_localize_utf8(strings[cnt2]);
                    else fullfile[cnt] += strings[cnt2];
                    if (cnt2 != size - 1) fullfile[cnt] += L"=";
                }
            }
            fwprintf_s(fp, L"%ls\n", fullfile[cnt].c_str());
        }
        fclose(fp);
    }
    return;

}
void Pseudo_inf_utf8(std::wstring path) {
    std::wstring buffer;
    std::wifstream in(path);
    std::vector<std::wstring> fullfile;
    if (in) // 有该文件
    {
        while (std::getline(in, buffer)) // line中不包括每行的换行符
        {
            fullfile.push_back((buffer));
        }
    }
    in.close();
    std::wofstream out(path);
    if (out) {
        bool pseudolocalize = false;
        for (int cnt = 0; cnt < fullfile.size(); cnt++) {
            replace_all(fullfile[cnt], L"\r", L"");
            if (_wcsicmp(fullfile[cnt].c_str(), L"[Strings.0409]") == 0)fullfile[cnt] = L"[Strings.0501]";
            if (!fullfile[cnt].empty() && fullfile[cnt][0] == ';') {
                if (fullfile[cnt].find(L"ocalizable") != std::wstring::npos) {
                    if (fullfile[cnt].find(L"on") == std::wstring::npos) pseudolocalize = true;
                    else pseudolocalize = false;
                }
            }
            else if (!fullfile[cnt].empty() && pseudolocalize) {
                std::vector<std::wstring> strings;
                mstr_split(strings, fullfile[cnt], L"\"");
                size_t size = strings.size();
                fullfile[cnt].clear();
                for (size_t cnt2 = 0; cnt2 < size; cnt2++) {
                    if (cnt2 % 2 == 1) fullfile[cnt] += Pseudo_localize_utf8(strings[cnt2]);
                    else fullfile[cnt] += strings[cnt2];
                    if (cnt2 != size - 1) fullfile[cnt] += L"\"";
                }
            }
            out << fullfile[cnt].c_str() << std::endl;
        }
        out.close();
    }
    return;

}
void Pseudo_ini_utf8(std::wstring path) {
    std::wstring buffer;
    std::wifstream in(path);
    std::vector<std::wstring> fullfile;
    if (in) // 有该文件
    {
        while (std::getline(in, buffer)) // line中不包括每行的换行符
        {
            fullfile.push_back((buffer));
        }
    }
    in.close();
    std::wofstream out(path);
    if (out) {
        bool pseudolocalize = false;
        for (int cnt = 0; cnt < fullfile.size(); cnt++) {
            replace_all(fullfile[cnt], L"\r", L"");
            if (fullfile[cnt].find(L"009=") != std::wstring::npos && !pseudolocalize) fullfile[cnt] = L"101=UNASSIGNED LANGUAGE ID";
            if (!fullfile[cnt].empty() && fullfile[cnt][0] == '[') {
                if (fullfile[cnt].find(L"bjects") != std::wstring::npos || fullfile[cnt].find(L"ext") != std::wstring::npos) {
                    pseudolocalize = true;

                }
                else pseudolocalize = false;
            }
            else if (!fullfile[cnt].empty() && pseudolocalize) {
                replace_all(fullfile[cnt], L"009", L"101");
                std::vector<std::wstring> strings;
                mstr_split(strings, fullfile[cnt], L"=");
                size_t size = strings.size();
                fullfile[cnt].clear();
                for (size_t cnt2 = 0; cnt2 < size; cnt2++) {
                    if (cnt2 % 2 == 1 && fullfile[cnt].find(L"_000_") == std::wstring::npos) fullfile[cnt] += Pseudo_localize(strings[cnt2]);
                    else fullfile[cnt] += strings[cnt2];
                    if (cnt2 != size - 1) fullfile[cnt] += L"=";
                }
            }
            out<< fullfile[cnt].c_str()<<std::endl;
        }
        out.close();
    }
    return;

}

void Pseudo_inf(std::wstring path) {
    if (IsUTF16File(path.c_str())) Pseudo_inf_utf16(path);
    else  Pseudo_inf_utf8(path);
}

void Pseudo_ini(std::wstring path) {
    if (IsUTF16File(path.c_str())) Pseudo_ini_utf16(path);
    else  Pseudo_ini_utf8(path);
}
