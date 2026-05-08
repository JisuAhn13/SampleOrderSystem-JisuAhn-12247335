#include "ConsoleUI.h"
#include <iostream>
#include <string>

namespace ConsoleUI {

static std::string rep(const std::string& s, int n)
{
    std::string r;
    for (int i = 0; i < n; ++i) r += s;
    return r;
}

int displayWidth(const std::string& s)
{
    int w = 0;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = (unsigned char)s[i];
        if      (c < 0x80) { w += 1; i += 1; }
        else if (c < 0xE0) { w += 1; i += 2; }
        else if (c < 0xF0) { w += 2; i += 3; }  // 한글·전각(CJK) = 2칸
        else               { w += 2; i += 4; }
    }
    return w;
}

void printBoxTop()
{
    std::cout << std::string(LEFT_PAD, ' ') << "╔" << rep("═", BOX_W) << "╗\n";
}

void printBoxMid()
{
    std::cout << std::string(LEFT_PAD, ' ') << "╠" << rep("═", BOX_W) << "╣\n";
}

void printBoxBot()
{
    std::cout << std::string(LEFT_PAD, ' ') << "╚" << rep("═", BOX_W) << "╝\n";
}

void printBoxLine(const std::string& content)
{
    int pad = BOX_W - displayWidth(content);
    if (pad < 0) pad = 0;
    std::cout << std::string(LEFT_PAD, ' ') << "║" << content
              << std::string(pad, ' ') << "║\n";
}

void printBoxEmpty()
{
    std::cout << std::string(LEFT_PAD, ' ') << "║" << std::string(BOX_W, ' ') << "║\n";
}

} // namespace ConsoleUI
