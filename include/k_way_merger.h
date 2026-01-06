#ifndef K_WAY_MERGER_H
#define K_WAY_MERGER_H

#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <filesystem>
#include <memory>
#include <iostream>
#include "ui_utils.h"

using namespace std;

class KWayMerger {
private:
    struct MinHeapNode {
        int value;
        int fileIndex;
        
        bool operator>(const MinHeapNode& other) const {
            return value > other.value;
        }
    };

    void cleanup(const vector<string>& chunkFiles, const string& tempDir) {
        using namespace TerminalUI;
        
        for (const auto& file : chunkFiles) {
            try {
                filesystem::remove(file);
            } catch (...) {
                cerr << YELLOW << "Warning: Cannot remove " << file << RESET << endl;
            }
        }
        
        try {
            filesystem::remove(tempDir);
        } catch (...) {}
    }

public:
    void merge(const vector<string>& sortedChunks, const string& outputFile, bool cleanupChunks = true) {
        using namespace TerminalUI;
        
        printHeader("PHASE 2: K-WAY MERGING");
        
        if (sortedChunks.empty()) {
            cerr << RED << "Error: No chunks to merge!" << RESET << endl;
            return;
        }
        
        priority_queue<MinHeapNode, vector<MinHeapNode>, greater<MinHeapNode>> minHeap;
        
        vector<unique_ptr<ifstream>> streams;
        for (const auto& file : sortedChunks) {
            streams.push_back(make_unique<ifstream>(file));
            if (!*streams.back()) {
                cerr << RED << "Error: Cannot open chunk " << file << RESET << endl;
            }
        }
        
        long long totalBytes = 0;
        for (const auto& file : sortedChunks) {
            totalBytes += filesystem::file_size(file);
        }
        long long processedBytes = 0;
        
        for (size_t i = 0; i < streams.size(); ++i) {
            int val;
            if (*streams[i] >> val) {
                minHeap.push({val, (int)i});
            }
        }
        
        ofstream outFile(outputFile);
        if (!outFile) {
            cerr << RED << "Error: Cannot create output file " << outputFile << RESET << endl;
            return;
        }
        
        int counter = 0;
        
        while (!minHeap.empty()) {
            MinHeapNode minNode = minHeap.top();
            minHeap.pop();
            
            outFile << minNode.value << " ";
            processedBytes += 6;
            
            if (++counter % 2000 == 0) {
                float progress = (float)processedBytes / totalBytes;
                if (progress > 1.0) progress = 1.0;
                drawProgressBar(progress, "Merging Files");
            }
            
            int nextVal;
            if (*streams[minNode.fileIndex] >> nextVal) {
                minHeap.push({nextVal, minNode.fileIndex});
            }
        }
        
        drawProgressBar(1.0, "Merging Files");
        outFile.close();
        streams.clear();
        
        cout << "\n" << GREEN << "✓ Merge Complete!" << RESET << endl;
        
        if (cleanupChunks) {
            string tempDir = sortedChunks[0].substr(0, sortedChunks[0].find_last_of("/\\") + 1);
            cleanup(sortedChunks, tempDir);
        }
    }

    bool verifyOutput(const string& filename) {
        ifstream inFile(filename);
        if (!inFile) return false;
        
        int prev, curr;
        if (!(inFile >> prev)) return true;
        
        while (inFile >> curr) {
            if (curr < prev) return false;
            prev = curr;
        }
        return true;
    }
};

#endif