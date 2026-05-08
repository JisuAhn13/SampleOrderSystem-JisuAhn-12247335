#pragma once
#include <string>

// 박스 테두리 문자와 디스플레이 폭 계산을 한 곳에서 관리해 출력 일관성을 보장한다.
namespace ConsoleUI {
    constexpr int SCREEN_W = 80;
    constexpr int BOX_W    = 58;
    constexpr int LEFT_PAD = (SCREEN_W - BOX_W - 2) / 2;  // = 10

    int  displayWidth(const std::string& s);
    void printBoxTop();
    void printBoxMid();
    void printBoxBot();
    void printBoxLine(const std::string& content);
    void printBoxEmpty();
}
