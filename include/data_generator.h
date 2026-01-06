#ifndef DATA_GENERATOR_H
#define DATA_GENERATOR_H

#include <string>
#include <fstream>
#include <random>
#include <iostream>
#include "ui_utils.h"

using namespace std;

class DataGenerator {
public:
    static void generateFile(const string& filename, size_t count, int minVal = 1, int maxVal = 100000) {
        using namespace TerminalUI;
        
        printHeader("GENERATING DATA");
        
        ofstream outFile(filename);
        if (!outFile) {
            cerr << RED << "Error: Cannot create file " << filename << RESET << endl;
            return;
        }
        
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(minVal, maxVal);

        for (size_t i = 0; i < count; ++i) {
            outFile << dis(gen) << " ";
            
            if (i % (count / 100 + 1) == 0) {
                drawProgressBar((float)i / count, "Generating");
            }
        }
        
        drawProgressBar(1.0, "Generating");
        outFile.close();
        
        cout << "\n" << GREEN << "✓ Created file: " << filename 
             << " (" << count << " integers)" << RESET << endl;
    }

    static void generateSortedFile(const string& filename, size_t count) {
        ofstream outFile(filename);
        for (size_t i = 1; i <= count; ++i) {
            outFile << i << " ";
        }
        outFile.close();
    }

    static void generateReversedFile(const string& filename, size_t count) {
        ofstream outFile(filename);
        for (int i = count; i >= 1; --i) {
            outFile << i << " ";
        }
        outFile.close();
    }

private:
    DataGenerator() = delete;
};

#endif