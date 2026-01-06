#include <iostream>
#include <chrono>
#include "include/ui_utils.h"
#include "include/data_generator.h"
#include "include/chunk_processor.h"
#include "include/k_way_merger.h"

using namespace std;
using namespace TerminalUI;

// ==========================================
// CONFIGURATION
// ==========================================
const int DATA_SIZE = 500000;           // 500k integers
const size_t CHUNK_SIZE = 10000;        // 10k integers per chunk
const string INPUT_FILE = "input_large.txt";
const string OUTPUT_FILE = "sorted_output.txt";
const string TEMP_DIR = "temp_chunks/";

// ==========================================
// MAIN PROGRAM
// ==========================================
int main() {
    clearScreen();
    printHeader("DSA PROJECT: BIG DATA FILE SORTER");
    
    cout << "   " << CYAN << "Team:" << RESET << " 3 Members\n";
    cout << "   " << CYAN << "Algorithm:" << RESET << " External Merge Sort\n";
    cout << "   " << CYAN << "Features:" << RESET << " Real-time RAM monitoring\n";
    cout << "\n   " << YELLOW << "Roles Distribution:" << RESET << "\n";
    cout << "\t24110098 - Ly Tran Gia Khang: Chunk Processor (Split large file)\n";
    cout << "\t24110151 - Nguyen Dang Vuong: Heap Sorter (Sort chunks in RAM)\n";
    cout << "\t24110140 - Doan Trong Trung: K-Way Merger\n";
    
    // ===== STEP 1: GENERATE TEST DATA =====
    waitForEnter();
    DataGenerator::generateFile(INPUT_FILE, DATA_SIZE);
    auto startTime = chrono::high_resolution_clock::now();
    
    // ===== STEP 2: CHUNK & SORT (Khang + Vuong) =====
    waitForEnter();
    ChunkProcessor processor(CHUNK_SIZE, TEMP_DIR);
    vector<string> sortedChunks = processor.processFile(INPUT_FILE);
    
    if (sortedChunks.empty()) {
        cerr << RED << "\nError: No chunks created. Exiting..." << RESET << endl;
        return 1;
    }
    
    // ===== STEP 3: MERGE (Trung) =====
    waitForEnter();
    KWayMerger merger;
    merger.merge(sortedChunks, OUTPUT_FILE, true);
    
    auto endTime = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = endTime - startTime;
    
    // ===== STEP 4: VERIFY & REPORT =====
    printHeader("VERIFICATION & REPORT");
    cout << CYAN << "Verifying output..." << RESET << endl;
    bool isValid = merger.verifyOutput(OUTPUT_FILE);
    
    if (isValid) {
        cout << GREEN << "-> [RIGHT] Output is correctly sorted!" << RESET << "\n\n";
    } else {
        cout << RED << "-> [WRONG] Output is NOT sorted correctly!" << RESET << "\n\n";
    }
    
    // Statistics
    printStat("Total Integers", to_string(DATA_SIZE));
    printStat("Chunk Size", to_string(CHUNK_SIZE));
    printStat("Total Chunks", to_string(sortedChunks.size()));
    printStat("Execution Time", to_string(elapsed.count()) + " seconds");
    printStat("Algorithm", "External Merge Sort");
    printStat("Time Complexity", "O(N log N)");
    printStat("Space Complexity", "O(CHUNK_SIZE)");
    
    cout << "\n" << YELLOW << "Output file: " << OUTPUT_FILE << RESET << endl;
    cout << GREEN << "-> Program completed successfully!" << RESET << endl;
    cout << "\n";
    
    return 0;
}
