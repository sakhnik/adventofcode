#pragma once

#include <iostream>
#include <cstring>
#include <string_view>

class Printer
{
public:
    static Printer& Instance()
    {
        static Printer instance;
        return instance;
    }

    template <typename MsgT>
    static void Print(std::string_view fname, const char *suffix, MsgT msg)
    {
        Print(fname, suffix, std::to_string(msg));
    }

    static void Print(std::string_view fname, const char *suffix, const std::string &msg)
    {
        Instance()._Print(fname, suffix, msg);
    }

    static void Print(std::string_view fname, const char *suffix, std::string_view msg)
    {
        Instance()._Print(fname, suffix, msg);
    }

private:
    Printer() = default;
    Printer(const Printer &) = delete;
    Printer(Printer &&) = delete;
    Printer& operator=(const Printer &) = delete;
    Printer& operator=(Printer &&) = delete;

    void _Print(std::string_view fname, const char *suffix, std::string_view msg)
    {
        auto base_start = fname.find_last_of("/\\") + 1;
        auto year_start = fname.find_last_of("/\\", base_start - 2) + 1;
        auto base_end = fname.find(".cpp", base_start);
        auto year = fname.substr(year_start, base_start - year_start - 1);
        auto base = fname.substr(base_start, base_end - base_start);

        std::cout << year << '-' << base;
        if (suffix)
        {
            std::cout << '.' << suffix;
        }
        std::cout << ": " << msg << "\n";
    }
};
