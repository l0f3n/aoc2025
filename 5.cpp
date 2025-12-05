#include <iostream>
#include <string>
#include <vector>
#include <ranges>
#include <algorithm>
#include <sstream>
#include <cstdint>

struct Range {
    int64_t start;
    int64_t end;
    
    bool contains(int64_t id) const {
        return id >= start && id <= end;
    }
};

// Parse a range from "start-end" format
Range parseRange(const std::string& line) {
    auto dashPos = line.find('-');
    int64_t start = std::stoll(line.substr(0, dashPos));
    int64_t end = std::stoll(line.substr(dashPos + 1));
    return {start, end};
}

// Check if an ID is fresh (in any range)
bool isFresh(int64_t id, const std::vector<Range>& ranges) {
    return std::ranges::any_of(ranges, [id](const Range& r) { return r.contains(id); });
}

int main() {
    std::vector<Range> ranges;
    std::vector<int64_t> ingredientIDs;
    std::string line;
    bool readingRanges = true;
    
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            readingRanges = false;
            continue;
        }
        
        if (readingRanges) {
            ranges.push_back(parseRange(line));
        } else {
            ingredientIDs.push_back(std::stoll(line));
        }
    }
    
    // Part 1: Count fresh ingredient IDs
    int part1 = std::ranges::fold_left(
        ingredientIDs | std::views::transform([&ranges](int64_t id) { 
            return isFresh(id, ranges) ? 1 : 0; 
        }),
        0,
        std::plus{}
    );
    
    // Part 2: Count all distinct IDs covered by ranges (merge overlapping ranges)
    std::vector<Range> sortedRanges = ranges;
    std::ranges::sort(sortedRanges, [](const Range& a, const Range& b) {
        return a.start < b.start || (a.start == b.start && a.end < b.end);
    });
    
    std::vector<Range> merged;
    if (!sortedRanges.empty()) {
        merged.push_back(sortedRanges[0]);
        for (const auto& range : sortedRanges | std::views::drop(1)) {
            if (range.start <= merged.back().end + 1) {
                // Overlaps or adjacent, merge
                merged.back().end = std::max(merged.back().end, range.end);
            } else {
                merged.push_back(range);
            }
        }
    }
    
    // Sum sizes of merged ranges
    int64_t part2 = std::ranges::fold_left(
        merged | std::views::transform([](const Range& r) { 
            return r.end - r.start + 1; 
        }),
        int64_t{0},
        std::plus{}
    );
    
    std::cout << part1 << std::endl;
    std::cout << part2 << std::endl;
    
    return 0;
}

