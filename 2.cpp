#include <iostream>
#include <string>
#include <vector>
#include <ranges>
#include <algorithm>
#include <sstream>
#include <numeric>

struct Range {
    int64_t start;
    int64_t end;
};

// Parse a single range from "start-end" format
Range parseRange(const std::string& str) {
    auto dashPos = str.find('-');
    int64_t start = std::stoll(str.substr(0, dashPos));
    int64_t end = std::stoll(str.substr(dashPos + 1));
    return {start, end};
}

// Parse all ranges from comma-separated string
std::vector<Range> parseRanges(const std::string& line) {
    std::vector<Range> ranges;
    std::istringstream iss(line);
    std::string token;
    
    while (std::getline(iss, token, ',')) {
        if (!token.empty()) {
            ranges.push_back(parseRange(token));
        }
    }
    
    return ranges;
}

// Check if a number is an invalid ID (made of a sequence repeated at least twice)
bool isInvalidID(int64_t num, bool part2 = false) {
    std::string str = std::to_string(num);
    size_t len = str.length();
    
    if (!part2) {
        // Part 1: Must have even number of digits and first half equals second half
        if (len % 2 != 0) {
            return false;
        }
        size_t half = len / 2;
        return str.substr(0, half) == str.substr(half);
    } else {
        // Part 2: Check if made of a sequence repeated at least twice
        // Try all possible sequence lengths from 1 to len/2
        for (size_t seqLen = 1; seqLen <= len / 2; seqLen++) {
            if (len % seqLen != 0) continue;  // Must divide evenly
            
            std::string sequence = str.substr(0, seqLen);
            bool valid = true;
            
            // Check if the entire string is made of this sequence repeated
            for (size_t i = seqLen; i < len; i += seqLen) {
                if (str.substr(i, seqLen) != sequence) {
                    valid = false;
                    break;
                }
            }
            
            if (valid) {
                return true;  // Found a valid repetition pattern
            }
        }
        return false;
    }
}

// Count and sum invalid IDs in a range
int64_t processRange(const Range& range, bool part2 = false) {
    auto numbers = std::views::iota(range.start, range.end + 1);
    
    return std::ranges::fold_left(
        numbers,
        int64_t{0},
        [part2](int64_t sum, int64_t num) {
            return sum + (isInvalidID(num, part2) ? num : 0);
        }
    );
}

int main() {
    std::string line;
    std::getline(std::cin, line);
    
    if (line.empty()) {
        std::cout << "0\n0\n";
        return 0;
    }
    
    auto ranges = parseRanges(line);
    
    // Part 1: Sum of all invalid IDs (sequence repeated exactly twice)
    int64_t part1 = std::ranges::fold_left(
        ranges | std::views::transform([](const Range& r) { return processRange(r, false); }),
        int64_t{0},
        std::plus{}
    );
    
    // Part 2: Sum of all invalid IDs (sequence repeated at least twice)
    int64_t part2 = std::ranges::fold_left(
        ranges | std::views::transform([](const Range& r) { return processRange(r, true); }),
        int64_t{0},
        std::plus{}
    );
    
    std::cout << part1 << std::endl;
    std::cout << part2 << std::endl;
    
    return 0;
}
