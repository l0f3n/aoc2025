#include <iostream>
#include <string>
#include <vector>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <cstdint>

// Find maximum number from selecting exactly k digits
// The number is formed in position order
int64_t findMaxJoltage(const std::string& bank, int k) {
    if (k == 2) {
        // Part 1: Try all pairs
        int maxJoltage = 0;
        for (size_t i = 0; i < bank.length(); i++) {
            for (size_t j = i + 1; j < bank.length(); j++) {
                int num = (bank[i] - '0') * 10 + (bank[j] - '0');
                maxJoltage = std::max(maxJoltage, num);
            }
        }
        return maxJoltage;
    } else {
        // Part 2: Select k digits to form largest number
        // Use greedy approach: remove (n-k) digits to leave largest number
        int toRemove = bank.length() - k;
        std::string result;
        
        for (char digit : bank) {
            // Remove digits from result while we can and current digit is larger
            while (!result.empty() && toRemove > 0 && digit > result.back()) {
                result.pop_back();
                toRemove--;
            }
            result.push_back(digit);
        }
        
        // If we still need to remove more, remove from the end
        while (toRemove > 0 && !result.empty()) {
            result.pop_back();
            toRemove--;
        }
        
        return std::stoll(result);
    }
}

int main() {
    std::vector<std::string> banks;
    std::string line;
    
    while (std::getline(std::cin, line)) {
        if (!line.empty()) {
            banks.push_back(line);
        }
    }
    
    // Part 1: Sum of maximum joltage from each bank (2 digits)
    int64_t part1 = std::ranges::fold_left(
        banks | std::views::transform([](const std::string& b) { return findMaxJoltage(b, 2); }),
        int64_t{0},
        std::plus{}
    );
    
    // Part 2: Sum of maximum joltage from each bank (12 digits)
    int64_t part2 = std::ranges::fold_left(
        banks | std::views::transform([](const std::string& b) { return findMaxJoltage(b, 12); }),
        int64_t{0},
        std::plus{}
    );
    
    std::cout << part1 << std::endl;
    std::cout << part2 << std::endl;
    
    return 0;
}

