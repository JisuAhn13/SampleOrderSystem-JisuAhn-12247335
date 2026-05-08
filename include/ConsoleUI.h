#pragma once
#include <string>

// 콘솔 박스 출력 유틸리티
// MenuUI, Monitor, ShippingManager 등 모든 레이어에서 공용으로 사용
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
