#pragma once

#include <iostream>
#include <bitset>

#include "Parser/utils/GameStateContainer.cpp"

using namespace std;

class Utils {
public:
    static void displayBoard(const GameStateContainer* container) {
        map<unsigned char, bitset<64>> parsedBoards = container->getBoards();
        for (int i = 63; i >= 0; i--) {
            bool printed = false;

            for (const auto& element: parsedBoards) {
                if (element.second[i] != 0) {
                    cout << element.first;
                    printed = true;
                    break;
                }
            }

            if (!printed) {
                cout << " ";
            }

            if (i % 8 == 0) {
                cout << endl;
            }
        }
    }

};