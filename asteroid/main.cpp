#include <bits/stdc++.h>
using namespace std;

struct Asteroid
{
    int w, h, digLimit;
    vector<string> grid;
};

Asteroid read_asteroid(istream &in)
{
    Asteroid a;
    in >> a.w >> a.h >> a.digLimit;
    int total = a.h + 2;
    a.grid.resize(total);
    for (int i = 0; i < total; ++i)
    {
        in >> a.grid[i];
    }
    return a;
}

void find_outpost(const Asteroid &a, int &s_r, int &s_c)
{
    s_r = 0;
    s_c = -1;
    const string &row = a.grid[0];
    for (int c = 0; c < (int)row.size(); ++c)
        if (row[c] == 'S')
            s_c = c;
}

int count_dug(const Asteroid &a)
{
    int count = 0;
    for (const auto &row : a.grid)
        count += std::count(row.begin(), row.end(), 'X');
    return count;
}

void dig_vertical(Asteroid &a, int s_c)
{
    // For h%3==0: ensure vertical coverage by completing vertical columns
    // Horizontal rows are secondary - only add if budget allows
    
    vector<int> col;
    for (int c = 0; c < (int)a.grid[1].size(); ++c)
        if (a.grid[1][c] == ':')
            col.push_back(c);

    if (col.empty()) return;
    
    int dug = count_dug(a);
    
    // Step 1: Connect from outpost down to row 2
    for (int r = 1; r <= min(2, a.h) && dug < a.digLimit; ++r)
    {
        if (a.grid[r][s_c] == ':')
        {
            a.grid[r][s_c] = 'X';
            dug++;
        }
    }
    
    // Step 2: Calculate budget - reserve space for vertical columns
    vector<int> vert_indices;
    for (int i = 1; i < (int)col.size(); i += 3)
        vert_indices.push_back(i);
    
    // Count cells needed for full vertical columns
    int vert_needed = 0;
    for (int idx : vert_indices)
    {
        int c = col[idx];
        for (int r = 1; r <= a.h; ++r)
            if (a.grid[r][c] == ':')
                vert_needed++;
    }
    
    // Step 3: Dig first horizontal row only if we can still afford vertical columns after
    int horiz_row2_needed = 0;
    for (int c : col)
        if (a.grid[2][c] == ':')
            horiz_row2_needed++;
    
    if (dug + horiz_row2_needed + vert_needed <= a.digLimit)
    {
        // We can afford both - dig horizontal row 2
        for (int c : col)
        {
            if (dug >= a.digLimit) break;
            if (a.grid[2][c] == ':')
            {
                a.grid[2][c] = 'X';
                dug++;
            }
        }
    }
    
    // Step 4: Dig all vertical columns completely
    for (int idx : vert_indices)
    {
        int c = col[idx];
        for (int r = 1; r <= a.h && dug < a.digLimit; ++r)
        {
            if (a.grid[r][c] == ':')
            {
                a.grid[r][c] = 'X';
                dug++;
            }
        }
    }
    
    // Step 5: Add horizontal rows if budget remains
    for (int r = 2; r <= a.h && dug < a.digLimit; r += 3)
    {
        if (a.grid[r][col[0]] == 'X') continue; // Already dug
        
        int needed = 0;
        for (int c : col)
            if (a.grid[r][c] == ':')
                needed++;
        
        if (dug + needed <= a.digLimit)
        {
            for (int c : col)
            {
                if (a.grid[r][c] == ':')
                {
                    a.grid[r][c] = 'X';
                    dug++;
                }
            }
        }
    }
}

void dig_horizontal(Asteroid &a, int s_c)
{
    // For w%3==0: dig pattern ensuring all parts stay connected
    // Strategy: dig at least one vertical connector, then horizontal rows, then more verticals
    
    vector<int> col;
    for (int c = 0; c < (int)a.grid[1].size(); ++c)
        if (a.grid[1][c] == ':')
            col.push_back(c);

    if (col.empty()) return;

    int dug = count_dug(a);

    // Connect from outpost down to row 2
    for (int r = 1; r <= min(2, a.h) && dug < a.digLimit; ++r)
    {
        if (a.grid[r][s_c] == ':')
        {
            a.grid[r][s_c] = 'X';
            dug++;
        }
    }

    // CRITICAL: Dig at least the first vertical connector column to ensure connectivity
    if (col.size() > 1)
    {
        int c = col[1]; // Column at index 1 (every 3rd starting pattern)
        for (int r = 1; r <= a.h && dug < a.digLimit; ++r)
        {
            if (a.grid[r][c] == ':')
            {
                a.grid[r][c] = 'X';
                dug++;
            }
        }
    }

    // Dig main horizontal rows (every 3rd row)
    for (int r = 2; r <= a.h && dug < a.digLimit; r += 3)
    {
        int needed = 0;
        for (int c : col)
            if (a.grid[r][c] == ':')
                needed++;
        
        if (dug + needed <= a.digLimit)
        {
            for (int c : col)
            {
                if (a.grid[r][c] == ':')
                {
                    a.grid[r][c] = 'X';
                    dug++;
                }
            }
        }
        else
        {
            break;
        }
    }

    // Add additional vertical connectors (starting from index 4, then every 3)
    for (int i = 4; i < (int)col.size() && dug < a.digLimit; i += 3)
    {
        int c = col[i];
        for (int r = 1; r <= a.h && dug < a.digLimit; ++r)
        {
            if (a.grid[r][c] == ':')
            {
                a.grid[r][c] = 'X';
                dug++;
            }
        }
    }
}

void maybe_add_bonus_dig(Asteroid &a)
{
    int dug = count_dug(a);
    if (dug >= a.digLimit)
        return;

    // Priority: find cells that are NOT adjacent to any tunnel
    // These are the most important to fill for coverage
    vector<pair<int,int>> uncovered;
    for (int r = 1; r <= a.h; ++r)
    {
        for (int c = 0; c < (int)a.grid[r].size(); ++c)
        {
            if (a.grid[r][c] == ':')
            {
                // Check if adjacent to any X
                bool has_adjacent = false;
                for (auto [dr, dc] : vector<pair<int,int>>{{-1,0}, {1,0}, {0,-1}, {0,1}})
                {
                    int nr = r + dr, nc = c + dc;
                    if (nr >= 0 && nr < (int)a.grid.size() && nc >= 0 && nc < (int)a.grid[nr].size())
                    {
                        if (a.grid[nr][nc] == 'X')
                        {
                            has_adjacent = true;
                            break;
                        }
                    }
                }
                if (!has_adjacent)
                {
                    uncovered.push_back({r, c});
                }
            }
        }
    }
    
    // Fill uncovered cells first
    for (auto [r, c] : uncovered)
    {
        if (dug >= a.digLimit) return;
        a.grid[r][c] = 'X';
        dug++;
    }
    
    // Then fill any remaining : cells
    for (int r = 1; r <= a.h; ++r)
    {
        for (int c = 0; c < (int)a.grid[r].size(); ++c)
        {
            if (dug >= a.digLimit) return;
            if (a.grid[r][c] == ':')
            {
                a.grid[r][c] = 'X';
                dug++;
            }
        }
    }
}

void solve(Asteroid &a)
{
    int s_r, s_c;
    find_outpost(a, s_r, s_c);

    // Choose strategy based on which dimension is divisible by 3
    if (a.w % 3 == 0)
        dig_horizontal(a, s_c);  // Width divisible by 3 -> horizontal main tunnels
    else if (a.h % 3 == 0)
        dig_vertical(a, s_c);    // Height divisible by 3 -> vertical main tunnels
    else
    {
        // Fallback: use based on aspect ratio
        if (a.w == 3)
            dig_vertical(a, s_c);
        else
            dig_horizontal(a, s_c);
    }

    maybe_add_bonus_dig(a);
}

void write_asteroid(const Asteroid &a, ostream &out, bool extra = true)
{
    for (const auto &row : a.grid)
        out << row << '\n';
    if (extra)
        out << '\n';
}

int main()
{
    ifstream in("input.in");
    ofstream out("output.out");

    int N;
    in >> N;
    for (int i = 0; i < N; ++i)
    {
        Asteroid a = read_asteroid(in);
        solve(a);
        write_asteroid(a, out, i + 1 < N);
    }
    return 0;
}
// 