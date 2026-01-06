#ifndef CHUNK_PROCESSOR_H
#define CHUNK_PROCESSOR_H

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <iostream>
#include "heap_sorter.h"
#include "ui_utils.h"

using namespace std;

class ChunkProcessor {
private:
    size_t m_chunkSize;
    string m_tempDir;
    HeapSorter m_sorter;

    void ensureTempDirExists() {
        if (!filesystem::exists(m_tempDir)) {
            filesystem::create_directory(m_tempDir);
        }
    }

    void writeChunk(const vector<int>& data, const string& filename) {
        ofstream outFile(filename);
        if (!outFile) {
            cerr << TerminalUI::RED << "Error: Cannot write chunk " 
                 << filename << TerminalUI::RESET << endl;
            return;
        }
        
        for (int val : data) {
            outFile << val << " ";
        }
        outFile.close();
    }

public:
    ChunkProcessor(size_t chunkSize, const string& tempDir)
        : m_chunkSize(chunkSize), m_tempDir(tempDir) {}

    vector<string> processFile(const string& inputFile) {
        using namespace TerminalUI;
        
        printHeader("PHASE 1: CHUNKING & SORTING");
        
        ensureTempDirExists();
        
        uintmax_t fileSize = filesystem::file_size(inputFile);
        uintmax_t bytesRead = 0;
        
        ifstream inFile(inputFile);
        if (!inFile) {
            cerr << RED << "Error: Cannot open file " << inputFile << RESET << endl;
            return {};
        }
        
        vector<string> chunkFiles;
        vector<int> buffer;
        int num;
        int chunkCount = 0;
        
        while (inFile >> num) {
            buffer.push_back(num);
            bytesRead += 6;
            
            if (buffer.size() >= m_chunkSize) {
                // 1. Show RAM Peak before extract
                float progress = (float)bytesRead / (fileSize > 0 ? fileSize : 1);
                drawProgressBar(progress, "Sorting (PEAK)");
                sleep_ms(50);

                // 2. Sort n Write
                m_sorter.heapSort(buffer);
                string chunkName = m_tempDir + "chunk_" + to_string(chunkCount++) + ".txt";
                writeChunk(buffer, chunkName);
                chunkFiles.push_back(chunkName);

                // 3. Extract RAM
                buffer.clear();
                buffer.shrink_to_fit();    // ...

                // 4. Show RAM Drop after extract
                drawProgressBar(progress, "Sorting (DROP)");
            }
        }

        // Processing remain chunks
        if (!buffer.empty()) {
            m_sorter.heapSort(buffer);
            string chunkName = m_tempDir + "chunk_" + to_string(chunkCount++) + ".txt";
            writeChunk(buffer, chunkName);
            chunkFiles.push_back(chunkName);
        }
        
        drawProgressBar(1.0, "Completed");
        cout << "\n" << GREEN << "-> Created " << chunkFiles.size() 
             << " sorted chunks" << RESET << endl;
        
        return chunkFiles;
    }

    size_t getChunkCount() const {
        if (!filesystem::exists(m_tempDir)) return 0;
        
        size_t count = 0;
        for (const auto& entry : filesystem::directory_iterator(m_tempDir)) {
            if (entry.path().extension() == ".txt") count++;
        }
        return count;
    }
};

#endif
