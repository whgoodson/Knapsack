#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <map>
#include <algorithm>
#include "Knapsack.h"
#include "HashTable.h"
#include "MaxHeap.h"
#include "gnuplot.h"

void setInputFiles(char selection);
void traditionalKnapsack();
void spaceEfficientKnapsack();
int MFKnapsack(int i, int j, std::vector<int> &values, std::vector<int> &weights, HashTable *hTable);
void greedyBuiltInSort();
void greedyMaxHeap();
void allApproaches();
void testKValues();

std::string filenames[3];
int dataSetNum = -1;

int main(int argc, char* argv[]) {
    char choice = ' ';

    std::string fileChoice = "NONE";

    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "Knapsack Solver\n\nby Warren Goodson and Erik Brownell\n";
    while(choice != 'q') {
        choice = '0';
        std::cout << "--------------------------------------------" << std::endl;
        std::cout << "Options:\n\n(s) Set Input Files (do this first)\n\n";
        std::cout << "(a) Traditional DP Approach\n(b) Space Efficient DP\n\n";
        std::cout << "(c) Greedy Approach (built-in sort)\n(d) Greedy Approach (max heap)\n\n(e) All Approaches\n\n(q) Quit\n";
        std::cin >> choice;

        char dataSelection = ' ';

        switch (choice) {
            case 's':
                std::cout << "Enter a number between 0 and 8 (inclusive) to select data set:" << std::endl;
                std::cin >> dataSelection;
                while(dataSelection < 48 || dataSelection > 56){
                    std::cout << "Error: please enter a number between 0 and 8, inclusive" << std::endl;
                    std::cin >> dataSelection;
                }
                setInputFiles(dataSelection);
                std::cout << "Data set 0" << dataSelection << " selected successfully." << std::endl;
                break;
            case 'a':
                traditionalKnapsack();
                break;
            case 'b':
                spaceEfficientKnapsack();
                break;
            case 'c':
                greedyBuiltInSort();
                break;
            case 'd':
                greedyMaxHeap();
                break;
            case 'e':
                allApproaches();
                break;
            case 't':
                testKValues();
            case 'q':
                break;
            default:
                std::cout << "Please choose either (a), (b), (c), (d), (e), or (s)\n";
        }
    }
    return 0;
}

void setInputFiles(char selection){

    char* dSet = &selection;
    dataSetNum = strtol(dSet, nullptr, 10);

    std::stringstream vName;
    vName << "./test_data/p0" << selection << "_v.txt";

    std::stringstream wName;
    wName << "./test_data/p0" << selection << "_w.txt";

    std::stringstream cName;
    cName << "./test_data/p0" << selection << "_c.txt";

    filenames[0] = vName.str();
    filenames[1] = wName.str();
    filenames[2] = cName.str();

}

void traditionalKnapsack(){
    if(filenames[0].empty()){
        std::cout << "Please select a data set (s)" << std::endl;
        return;
    }


    Knapsack kSack = Knapsack(filenames, dataSetNum);

//    std::cout << "Testing Knapsack data structure..." << std::endl;
    std::cout << "Data set: 0" << kSack.getDataSet() << std::endl;
    std::cout << "Total number of items: " << kSack.getValues().size() - 1 << std::endl;
    std::cout << "Capacity of Knapsack: " << kSack.getCapacity() << std::endl;
//    std::cout << std::endl;

    auto startTime = std::chrono::high_resolution_clock::now();

    std::cout << "Computing optimal value for knapsack..." << std::endl;
    std::cout << std::endl;
    std::vector<int> values = kSack.getValues();
    std::vector<int> weights = kSack.getWeights();

    int n = (int)values.size() - 1; // Account for vector starting at index 1
    int capacity = kSack.getCapacity();


    int F[n+1][capacity+1];
    // Zero out top row and leftmost column
    for(int i = 0; i <= n; i++)
        F[i][0] = 0;
    for(int j = 0; j <= capacity; j++)
        F[0][j] = 0;

    // Populate the table
    for(int i = 1; i <= n; i++){
        for(int j = 1; j <= capacity; j++){
            if(j - weights[i] < 0){
                // Not picked
                F[i][j] = F[i-1][j];
            } else {
                // Picked
                F[i][j] = std::max(F[i-1][j], values[i] + F[i-1][j-weights[i]]);
            }
        }
    }

    std::cout << "Traditional Dynamic Programming optimal value: " << F[n][capacity] << std::endl;

    // Backtrack to find the optimal subset
    std::vector<int> optimalSubset;

    int i = n;
    int j = capacity;
    while(i >= 0 && j > 0){
        if((j - weights[i] >= 0) && (values[i] + F[i-1][j-weights[i]] > F[i-1][j])){
            optimalSubset.push_back(i);
            j -= weights[i];
            i--;
        } else {
            i--;
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();

    auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

    std::cout << "Traditional Dynamic Programming optimal subset: {";
    for(int a = (int)optimalSubset.size() - 1; a > 0 ; a--){
        std::cout << optimalSubset[a] << ", ";
    }
    std::cout << optimalSubset[0] << "}" << std::endl;

    std::cout << "Traditional Dynamic Programming Time Taken: " << elapsedTime.count() << " microseconds" << std::endl;


}

void spaceEfficientKnapsack() {
    if(filenames[0].empty()){
        std::cout << "Please select a data set (s)" << std::endl;
        return;
    }

    Knapsack kSack = Knapsack(filenames, dataSetNum);

//    std::cout << "Testing Knapsack data structure..." << std::endl;
    std::cout << "Data set: 0" << kSack.getDataSet() << std::endl;
    std::cout << "Total number of items: " << kSack.getValues().size() - 1 << std::endl;
    std::cout << "Capacity of Knapsack: " << kSack.getCapacity() << std::endl;
//    std::cout << std::endl;

    auto startTime = std::chrono::high_resolution_clock::now();

    std::cout << "Computing optimal value for knapsack..." << std::endl;
    std::cout << std::endl;
    std::vector<int> values = kSack.getValues();
    std::vector<int> weights = kSack.getWeights();

    int n = (int)values.size() - 1; // Account for vector starting at index 1
    int capacity = kSack.getCapacity();

    int k = (n * capacity) / 10;

    HashTable hTable = HashTable(n, capacity, k);

    // Use MFKnapsack to get optimal value of subset

    int optimalValue = MFKnapsack(n, capacity, values, weights, &hTable);
    std::cout << "Space-efficient Dynamic Programming optimal value: " << optimalValue << std::endl << std::endl;

//    std::cout << "# of insertions: " << hTable.getNumInsertions() << std::endl;
//    std::cout << "# of collisions: " << hTable.countCollisions() << std::endl;
//    std::cout << "Size of table: " << hTable.size() << std::endl;

    // Backtrack to find the optimal subset
    std::vector<int> optimalSubset;

    int i = n;
    int j = capacity;
    while(i >= 0 && j > 0){
        if((j - weights[i] >= 0) && (values[i] + hTable.hashGet(i-1, j-weights[i]) > hTable.hashGet(i-1, j))){
            optimalSubset.push_back(i);
            j -= weights[i];
            i--;
        } else {
            i--;
        }
    }


    auto endTime = std::chrono::high_resolution_clock::now();

    auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);


    std::cout << "Space-efficient Dynamic Programming optimal subset: {";
    for(int a = (int)optimalSubset.size() - 1; a > 0 ; a--){
        std::cout << optimalSubset[a] << ", ";
    }
    std::cout << optimalSubset[0] << "}" << std::endl;

    std::cout << "Space-efficient Dynamic Programming Time Taken: " << elapsedTime.count() << " microseconds" << std::endl;

}

int MFKnapsack(int i, int j, std::vector<int> &values, std::vector<int> &weights, HashTable *hTable) {
    int value;
    if(hTable->hashGet(i, j) < 0 && i > 0){
        if(j < weights[i]){
            value = MFKnapsack(i - 1, j, values, weights, hTable);
        }
        else{
            int a = MFKnapsack(i - 1, j, values, weights, hTable);
            int b = values[i] + MFKnapsack(i - 1, j - weights[i], values, weights, hTable);
            value = std::max(a, b);
        }
        hTable->hashInsert(i, j, value);
    }
    return hTable->hashGet(i, j);
}

void greedyBuiltInSort(){
    if (filenames[0].empty()){
        std::cout << "Please select a data set (s)" << std::endl;
        return;
    }

    Knapsack kSack = Knapsack(filenames, dataSetNum);

//    std::cout << "Testing Knapsack data structure..." << std::endl;
    std::cout << "Data set: 0" << kSack.getDataSet() << std::endl;
    std::cout << "Total number of items: " << kSack.getValues().size() - 1 << std::endl;
    std::cout << "Capacity of Knapsack: " << kSack.getCapacity() << std::endl;
//    std::cout << std::endl;

    auto startTime = std::chrono::high_resolution_clock::now();

    std::cout << "Computing optimal value for knapsack..." << std::endl;
    std::cout << std::endl;
    std::vector<int> values = kSack.getValues();
    std::vector<int> weights = kSack.getWeights();

    std::map<float, int, std::greater<float> > ratios;
    std::vector<int> results;

    int n = (int)values.size() - 1; // Account for vector starting at index 1
    int capacity = kSack.getCapacity();

    // insert into ratios map
    for (int i = 1; i < n+1; i++){
        float f = (float)values[i] / weights[i];
        ratios.insert(std::pair<float, int>(f,i));
//        std::cout << "Ratio: " << f << ", i: " << i << std::endl;
    }

    // calculate subset
    float tmp_cap = 0;
    int total_val = 0;
    for (auto& r: ratios){
        int i = r.second;
        if ((tmp_cap + weights[i]) < capacity){
            total_val += values[i];
            tmp_cap += weights[i];
            results.push_back(i);
        }else
            break;
    }
    std::sort(results.begin(), results.end());


    auto endTime = std::chrono::high_resolution_clock::now();

    auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);


    // print
    std::cout << "Greedy Approach (built-in sort) Optimal value: " << total_val << std::endl;
    std::cout << "Greedy Approach (built-in sort) Optimal subset: {";
    for (int i = 0; i < results.size()-1; i++){
        std::cout << results[i] << ", ";
    }
    std::cout << results[results.size()-1] << "}" << std::endl;
    std::cout << "Greedy Approach (built-in sort) Time Taken: " << elapsedTime.count() << " microseconds" << std::endl;
    std::cout << std::endl;
}

void greedyMaxHeap(){
    if (filenames[0].empty()){
        std::cout << "Please select a data set (s)" << std::endl;
        return;
    }

    Knapsack kSack = Knapsack(filenames, dataSetNum);

//    std::cout << "Testing Knapsack data structure..." << std::endl;
    std::cout << "Data set: 0" << kSack.getDataSet() << std::endl;
    std::cout << "Total number of items: " << kSack.getValues().size() - 1 << std::endl;
    std::cout << "Capacity of Knapsack: " << kSack.getCapacity() << std::endl;
//    std::cout << std::endl;

    auto startTime = std::chrono::high_resolution_clock::now();

    std::cout << "Computing optimal value for knapsack..." << std::endl;
    std::cout << std::endl;
    std::vector<int> values = kSack.getValues();
    std::vector<int> weights = kSack.getWeights();

    std::vector<std::pair<float, int>> ratios;
    std::vector<int> results;

    int n = (int)values.size() - 1; // Account for vector starting at index 1
    int capacity = kSack.getCapacity();

    // insert into ratios vector
    ratios.emplace_back(std::pair<float, int>(-1.0f, -1));
    for (int i = 1; i <= n; i++){
        float f = (float)values[i] / weights[i];
        ratios.emplace_back(std::pair<float, int>(f,i));
    }

    // Build max heap from ratios
    MaxHeap mHeap = MaxHeap(ratios);

    std::vector<std::pair<float, int>> heapDeletions;
    for(int i = 0; i < n; i++){
        std::pair<float, int> current = mHeap.getDeleteMax();
//        std::cout << "Ratio: " << current.first << ", i: " << current.second << std::endl;
        heapDeletions.push_back(current);
    }


    // calculate subset
    float tmp_cap = 0;
    int total_val = 0;
    for (auto& r: heapDeletions){
        int i = r.second;
        if ((tmp_cap + weights[i]) < capacity){
            tmp_cap += weights[i];
            total_val += values[i];
            if(i > 0)
                results.push_back(i);
        }else
            break;
    }
    std::sort(results.begin(), results.end());


    auto endTime = std::chrono::high_resolution_clock::now();

    auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);


    // print
    std::cout << "Greedy Approach (max heap) Optimal value: " << total_val << std::endl;
    std::cout << "Greedy Approach (max heap) Optimal subset: {";
    for (int i = 0; i < results.size()-1; i++){
        std::cout << results[i] << ", ";
    }
    std::cout << results[results.size()-1] << "}" << std::endl;
    std::cout << "Greedy Approach (max heap) Time Taken: " << elapsedTime.count() << " microseconds" << std::endl;
    std::cout << std::endl;
}


void allApproaches(){
    if (filenames[0].empty()){
        std::cout << "Please select a data set (s)" << std::endl;
        return;
    }

    traditionalKnapsack();
    spaceEfficientKnapsack();
    greedyBuiltInSort();
    greedyMaxHeap();
}

// Test values of k on hash tables with the same data
void testKValues(){
    if(filenames[0].empty()){
        std::cout << "Please select a data set (s)" << std::endl;
        return;
    }

    std::ofstream testOutput;
    testOutput.open("hash_table_sizes_test.txt", std::ios::out );


    Knapsack kSack = Knapsack(filenames, dataSetNum);

    std::cout << "Computing optimal value for knapsack..." << std::endl;
    std::cout << std::endl;
    std::vector<int> values = kSack.getValues();
    std::vector<int> weights = kSack.getWeights();

    int n = (int)values.size() - 1; // Account for vector starting at index 1
    int capacity = kSack.getCapacity();

    // SET k
    int granularity = 1000;
    for(int i = (n * capacity) / granularity; i < n * capacity; i += (n * capacity) / granularity){
        HashTable hTable = HashTable(n, capacity, i);
        int collisions = hTable.countCollisions();
        std::cout << "Size: " << i << ", Collisions: " << collisions << std::endl;
        testOutput << collisions << std::endl;
    }

    testOutput.close();
}