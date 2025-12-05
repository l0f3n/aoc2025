#include <iostream>
#include <string>
#include <vector>
#include <ranges>
#include <algorithm>

// Count adjacent rolls of paper (8 directions)
int countAdjacentRolls(const std::vector<std::string>& grid, int row, int col) {
    int count = 0;
    int rows = grid.size();
    int cols = grid[0].size();
    
    // Check all 8 adjacent positions
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;  // Skip self
            
            int nr = row + dr;
            int nc = col + dc;
            
            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
                if (grid[nr][nc] == '@') {
                    count++;
                }
            }
        }
    }
    
    return count;
}

int main() {
    std::vector<std::string> grid;
    std::string line;
    
    while (std::getline(std::cin, line)) {
        if (!line.empty()) {
            grid.push_back(line);
        }
    }
    
    if (grid.empty()) {
        std::cout << "0\n0\n";
        return 0;
    }
    
    // Part 1: Count rolls accessible by forklift (< 4 adjacent rolls)
    int part1 = 0;
    for (size_t i = 0; i < grid.size(); i++) {
        for (size_t j = 0; j < grid[i].size(); j++) {
            if (grid[i][j] == '@') {
                int adjacent = countAdjacentRolls(grid, i, j);
                if (adjacent < 4) {
                    part1++;
                }
            }
        }
    }
    
    // Part 2: Simulate iterative removal process
    std::vector<std::string> workingGrid = grid;
    int part2 = 0;
    bool changed = true;
    
    while (changed) {
        changed = false;
        std::vector<std::pair<int, int>> toRemove;
        
        // Find all accessible rolls
        for (size_t i = 0; i < workingGrid.size(); i++) {
            for (size_t j = 0; j < workingGrid[i].size(); j++) {
                if (workingGrid[i][j] == '@') {
                    int adjacent = countAdjacentRolls(workingGrid, i, j);
                    if (adjacent < 4) {
                        toRemove.push_back({i, j});
                    }
                }
            }
        }
        
        // Remove accessible rolls
        for (auto [i, j] : toRemove) {
            workingGrid[i][j] = '.';
            part2++;
            changed = true;
        }
    }
    
    std::cout << part1 << std::endl;
    std::cout << part2 << std::endl;
    
    return 0;
}

