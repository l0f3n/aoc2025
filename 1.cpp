#include <iostream>
#include <string>
#include <utility>
#include <ranges>
#include <algorithm>
#include <numeric>

// Apply a rotation and count how many times we pass through 0 during it
std::pair<int, int> applyRotation(int start, int distance, char direction) {
    auto clicks = std::views::iota(0, distance);
    
    auto result = std::ranges::fold_left(clicks, 
        std::pair{start, 0},
        [direction](auto acc, auto) {
            auto [pos, count] = acc;
            int newPos = direction == 'R' 
                ? (pos + 1) % 100 
                : (pos - 1 + 100) % 100;
            return std::pair{newPos, count + (newPos == 0 ? 1 : 0)};
        });
    
    return result;
}

int main() {
    int position = 50;  // Dial starts at 50
    int count1 = 0;      // Part 1: Count zeros at end of rotations
    int count2 = 0;      // Part 2: Count zeros during rotations (including at end)
    
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        
        // Parse rotation: first character is L or R, rest is the number
        char direction = line[0];
        int distance = std::stoi(line.substr(1));
        
        // Apply rotation and count zeros during it (part 2)
        auto [newPosition, zeroCount] = applyRotation(position, distance, direction);
        count2 += zeroCount;
        position = newPosition;
        
        // Check if we're pointing at 0 at the end (part 1)
        if (position == 0) {
            count1++;
        }
    }
    
    std::cout << count1 << std::endl;
    std::cout << count2 << std::endl;
    
    return 0;
}

