#ifndef LISTENER_H
#define LISTENER_H

#include <Windows.h>
#include <atomic>
#include <thread>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <chrono>
#include "TypeWord.h"

class Listener {
public:
    std::unordered_set<int> keyModifier;
    std::unordered_set<int> keyModifierFull;
    std::unordered_set<int> keyNormal;
    std::unordered_set<int> keyNormalFull;
    std::unordered_set<WPARAM> keyMouse;
    std::unordered_set<WPARAM> keyMouseFull;

    bool flagHoldKeyControl;
    bool flagHoldKeyShift;
    bool flagHoldKeyMenu;
    bool flagRejectHook;
    int numHotkey = -1;

    POINT lastMousePoint;
    const int minMouseMoveDistance = 50;

    static Listener& getInstance();

    void start();
    void stop();
    void init();
    bool throttling();
    bool addChar(int vkCode);
    bool removeChar(int vkCode);
    bool moveLeft(int vkCode);
    bool moveRight(int vkCode);
    bool switchLangGlobal(int vkCode);
    bool switchLang(int vkCode);
    bool passSwitchWindow(int vkCode);
    bool passResetBuffer(int vkCode);
    bool resetBuffer(int vkCode);
    bool checkHotkey(int vkCode);
    bool checkFunction(int vkCode);
    bool checkModeRestore(int vkCode);
    void updateKeyPress(int vkCode);
    void updateKeyRelease(int vkCode);
    void updateMousePress(WPARAM wParam);
    void updateMouseRelease(WPARAM wParam);
    bool mouseMovedEnough(WPARAM wParam);
    void pressKey(int vkCode);
    void releaseKey(int vkCode);
    void checkKeyMouse(WPARAM vkCode);
    bool checkKeyModifierFull(int vkKey);
    bool checkKeyNormal(int vkKey);

    std::chrono::time_point<std::chrono::steady_clock> lastCall = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> lastCallControl;
    std::chrono::time_point<std::chrono::steady_clock> lastCallShift;
    std::chrono::time_point<std::chrono::steady_clock> lastCallMenu;
    const std::chrono::duration<long long, std::milli> minInterval = std::chrono::milliseconds(10);
    const std::chrono::duration<long long, std::milli> maxInterval = std::chrono::milliseconds(500);
    const std::chrono::duration<long long, std::milli> minIntervalSingle = std::chrono::milliseconds(300);
    const std::chrono::duration<long long, std::milli> minIntervalCombine = std::chrono::milliseconds(1000);

    Listener(const Listener&) = delete;
    Listener& operator=(const Listener&) = delete;

private:
    Listener();
    ~Listener();

    static LRESULT CALLBACK keyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK mouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);
    static HHOOK keyboardHook;
    static HHOOK mouseHook;
};

#endif // LISTENER_H
