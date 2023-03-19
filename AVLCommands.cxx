// AVLCommands.cxx
#include "AVL.h"
#include "json.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " input.json\n";
        return 1;
    }

    std::string inputFilename(argv[1]);
    std::ifstream inputFile(inputFilename);

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open input file \"" << inputFilename << "\"\n";
        return 1;
    }

    nlohmann::json inputJson;
    inputFile >> inputJson;

int numOps = inputJson["metadata"]["numOps"];
int maxDigits = std::to_string(numOps).length();

  AVL tree;

for (int i = 1; i <= numOps; ++i) {
    std::stringstream ss;
    ss << std::setw(maxDigits) << std::setfill('0') << i;
    std::string opKey = ss.str();
    // std::cout << opKey << std::endl;
    
    if (inputJson[opKey]["operation"] == "Insert") {
        tree.Insert(inputJson[opKey]["key"]);
    }
}

    std::cout << tree.JSON() << std::endl;

    return 0;
}
