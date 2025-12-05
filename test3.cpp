#include <iostream>
#include <string>
#include <algorithm>

int findMaxJoltage(const std::string& bank) {
    int maxJoltage = 0;
    for (size_t i = 0; i < bank.length(); i++) {
        for (size_t j = i + 1; j < bank.length(); j++) {
            int num1 = (bank[i] - '0') * 10 + (bank[j] - '0');
            int num2 = (bank[j] - '0') * 10 + (bank[i] - '0');
            maxJoltage = std::max({maxJoltage, num1, num2});
        }
    }
    return maxJoltage;
}

int main() {
    std::string banks[] = {"987654321111111", "811111111111119", "234234234234278", "818181911112111"};
    int sum = 0;
    for (auto& bank : banks) {
        int max = findMaxJoltage(bank);
        std::cout << bank << " -> " << max << std::endl;
        sum += max;
    }
    std::cout << "Total: " << sum << std::endl;
    return 0;
}
