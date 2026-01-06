#ifndef UI_UTILS_H
#define UI_UTILS_H

#include <iostream>
#include <iomanip>
#include <string>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#endif

using namespace std;

namespace TerminalUI {
    const string RESET = "\033[0m";
    const string RED = "\033[31m";
    const string GREEN = "\033[32m";
    const string YELLOW = "\033[33m";
    const string BLUE = "\033[34m";
    const string CYAN = "\033[36m";
    const string BOLD = "\033[1m";

    inline size_t getCurrentRAM_KB() {
    #ifdef _WIN32
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
            return pmc.WorkingSetSize / 1024;
        }
    #endif
        return 0;
    }

    inline void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    inline void printHeader(const string& title) {
        cout << BLUE << BOLD 
             << "\n============================================================\n";
        cout << "   " << title;
        cout << "\n============================================================\n" 
             << RESET;
    }

    inline void drawProgressBar(float progress, const string& message) {
        int barWidth = 25;
        
        size_t currentRAM = getCurrentRAM_KB();
        string ramColor = GREEN;
        if (currentRAM > 5000) ramColor = YELLOW;

        cout << "\r" << CYAN << left << setw(20) << message 
             << RESET << " [";
        
        int pos = barWidth * progress;
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) cout << "=";
            else if (i == pos) cout << ">";
            else cout << " ";
        }
        
        cout << "] " << setw(3) << int(progress * 100.0) << "% ";
        cout << "| RAM: " << ramColor << setw(6) << currentRAM 
             << " KB" << RESET << "    " << flush;
    }

    inline void waitForEnter() {
        cout << YELLOW << "\n\n[Press ENTER to continue...]" << RESET;
        cin.get();
    }

    inline void printStat(const string& label, const string& value) {
        cout << left << setw(25) << label 
             << ": " << GREEN << value << RESET << endl;
    }

    inline void sleep_ms(int milliseconds) {
        this_thread::sleep_for(chrono::milliseconds(milliseconds));
    }
}

#endif