#include "ConsoleUI.h"
#include <iostream>
#include <string>

namespace ConsoleUI {

static std::string repeatString(const std::string& s, int n)
{
    std::string result;
    result.reserve(s.size() * static_cast<std::size_t>(n));
    for (int i = 0; i < n; ++i) result += s;
    return result;
}

int displayWidth(const std::string& s)
{
    int width = 0;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char byte = (unsigned char)s[i];
        if      (byte < 0x80) { width += 1; i += 1; }
        else if (byte < 0xE0) { width += 1; i += 2; }
        else if (byte < 0xF0) { width += 2; i += 3; }  // 한글·전각(CJK) = 2칸
        else                  { width += 2; i += 4; }
    }
    return width;
}

void printBoxTop()
{
    std::cout << std::string(LEFT_PAD, ' ') << "╔" << repeatString("═", BOX_W) << "╗\n";
}

void printBoxMid()
{
    std::cout << std::string(LEFT_PAD, ' ') << "╠" << repeatString("═", BOX_W) << "╣\n";
}

void printBoxBot()
{
    std::cout << std::string(LEFT_PAD, ' ') << "╚" << repeatString("═", BOX_W) << "╝\n";
}

void printBoxLine(const std::string& content)
{
    int rightPadding = BOX_W - displayWidth(content);
    if (rightPadding < 0) rightPadding = 0;
    std::cout << std::string(LEFT_PAD, ' ') << "║" << content
              << std::string(rightPadding, ' ') << "║\n";
}

void printBoxEmpty()
{
    std::cout << std::string(LEFT_PAD, ' ') << "║" << std::string(BOX_W, ' ') << "║\n";
}

} // namespace ConsoleUI
