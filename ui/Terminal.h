#pragma once

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <io.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#else
#include <thread>
#include <unistd.h>
#endif

namespace ui {

class Terminal {
public:
    enum class Animation { Full, Reduced, Off };
    enum class Color { Green, Cyan, Yellow, Red, Grey, White, Blue };
    struct Timing {
        static constexpr int BootLine = 115;
        static constexpr int TitleCharacter = 18;
        static constexpr int BootProgressStep = 52;
        static constexpr int MenuReveal = 32;
        static constexpr int MenuRow = 50;
        static constexpr int DsaMenuRow = 60;
        static constexpr int EventHighlight = 45;
        static constexpr int EventReplay = 110;
        static constexpr int SegmentStep = 250;
        static constexpr int ReplaySegment = 220;
        static constexpr int ReportReveal = 80;
        static constexpr int ReportPause = 120;
    };

private:
    Animation animation_{Animation::Full};
    bool ansi_{false};
    bool ansiAvailable_{false};
#ifdef _WIN32
    HANDLE output_{INVALID_HANDLE_VALUE};
    DWORD originalMode_{0};
    bool changedMode_{false};
#endif

    static const char* code(Color color) {
        switch (color) {
            case Color::Green: return "\033[92m";
            case Color::Cyan: return "\033[96m";
            case Color::Yellow: return "\033[93m";
            case Color::Red: return "\033[91m";
            case Color::Grey: return "\033[90m";
            case Color::White: return "\033[97m";
            case Color::Blue: return "\033[94m";
        }
        return "\033[0m";
    }

public:
    Terminal() {
        const char* noColor = std::getenv("NO_COLOR");
        const char* term = std::getenv("TERM");
        if (noColor || (term && std::string(term) == "dumb")) return;
#ifdef _WIN32
        output_ = GetStdHandle(STD_OUTPUT_HANDLE);
        if (output_ != INVALID_HANDLE_VALUE && GetConsoleMode(output_, &originalMode_)) {
            const DWORD vtMode = originalMode_ | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            ansi_ = SetConsoleMode(output_, vtMode) != 0;
            changedMode_ = ansi_;
        }
#else
        ansi_ = ::isatty(STDOUT_FILENO) != 0;
#endif
        ansiAvailable_ = ansi_;
        const char* setting = std::getenv("DOOMSDAY_ANIMATION");
        if (setting && std::string(setting) == "off") animation_ = Animation::Off;
        else if (setting && std::string(setting) == "reduced") animation_ = Animation::Reduced;
    }

    ~Terminal() {
#ifdef _WIN32
        if (changedMode_) SetConsoleMode(output_, originalMode_);
#endif
        if (ansi_) std::cout << "\033[0m" << std::flush;
    }

    Terminal(const Terminal&) = delete;
    Terminal& operator=(const Terminal&) = delete;

    bool ansiEnabled() const noexcept { return ansi_; }
    Animation animation() const noexcept { return animation_; }
    void setAnimation(Animation value) noexcept { animation_ = value; }
    void setAnsi(bool value) noexcept { ansi_ = value && ansiAvailable_; }

    std::string paint(const std::string& value, Color color) const {
        return ansi_ ? std::string(code(color)) + value + "\033[0m" : value;
    }
    void write(const std::string& value, Color color) const { std::cout << paint(value, color); }

    void delay(int fullModeMilliseconds) const {
        if (animation_ == Animation::Off) return;
        const int duration = animation_ == Animation::Reduced ? fullModeMilliseconds / 4 : fullModeMilliseconds;
        if (duration <= 0) return;
#ifdef _WIN32
        Sleep(static_cast<DWORD>(duration));
#else
        std::this_thread::sleep_for(std::chrono::milliseconds(duration));
#endif
    }

    void revealLine(const std::string& value, Color color, int delayMilliseconds = Timing::MenuReveal) const {
        write(value, color);
        std::cout << '\n' << std::flush;
        delay(delayMilliseconds);
    }

    void typewriter(const std::string& value, Color color, int characterDelayMilliseconds = Timing::TitleCharacter) const {
        if (animation_ == Animation::Off) { write(value, color); std::cout << '\n'; return; }
        for (char ch : value) {
            write(std::string(1, ch), color);
            std::cout.flush();
            delay(characterDelayMilliseconds);
        }
        std::cout << '\n';
    }

    void progressBar(std::size_t width, int stepDelayMilliseconds) const {
        if (animation_ == Animation::Off) {
            std::cout << paint("[" + std::string(width, '=') + "] 100%\n", Color::Cyan);
            return;
        }
        std::cout << paint("[", Color::Grey);
        for (std::size_t i = 0; i < width; ++i) {
            write("=", Color::Cyan);
            std::cout.flush();
            delay(stepDelayMilliseconds);
        }
        std::cout << paint("] 100%\n", Color::Grey);
    }

    void revealGantt(const std::string& value, int delayMilliseconds) const {
        write(value, Color::Cyan);
        std::cout << std::flush;
        delay(delayMilliseconds);
    }
};

}  // namespace ui
