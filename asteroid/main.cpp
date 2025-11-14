#include <bits/stdc++.h>

constexpr int WIDTH = 3;

struct Asteroid {
    int w{}, h{}, digLimit{};
    std::vector<std::string> grid;
};

Asteroid read_asteroid(std::istream &in)
{
    Asteroid asteroid;
    in >> asteroid.w >> asteroid.h >> asteroid.digLimit;
    int total_rows = asteroid.h + 2;
    asteroid.grid.resize(total_rows);

    for (int i = 0; i < total_rows; ++i)
    {
        in >> asteroid.grid[i];
    }

    return asteroid;
}

std::pair<int, int> find_outpost(const Asteroid &a)
{
    for (int r = 0; r < (int)a.grid.size(); ++r)
    {
        auto pos = a.grid[r].find('S');
        if (pos != std::string::npos)
        {
            return {r, (int)pos};
        }
    }
    return {-1, -1};
}

void dig_vertical_highway(Asteroid &a, int s_col)
{
    std::vector<int> minableCols;
    for (int c = 0; c < (int)a.grid[1].size(); ++c)
    {
        if (a.grid[1][c] == ':')
        {
            minableCols.push_back(c);
        }
    }
    if (minableCols.size() != 3)
        return;

    int left = minableCols[0];
    int mid = minableCols[1];
    int right = minableCols[2];

    if (s_col == mid)
    {
        for (int r = 1; r <= a.h; ++r)
        {
            if (a.grid[r][mid] == ':')
                a.grid[r][mid] = 'X';
        }
    }
    else
    {
        int side = s_col;

        if (a.grid[1][side] == ':')
            a.grid[1][side] = 'X';
        if (a.grid[1][mid] == ':')
            a.grid[1][mid] = 'X';

        for (int r = 2; r <= a.h; ++r)
        {
            if (a.grid[r][mid] == ':')
                a.grid[r][mid] = 'X';
        }
    }
}

void dig_horizontal_highway(Asteroid &a, int s_col)
{
    if (a.w == 3)
    {
        for (int r = 1; r <= a.h; ++r)
        {
            if (a.grid[r][s_col] == ':')
                a.grid[r][s_col] = 'X';
        }
        return;
    }

    std::vector<int> minableCols;
    for (int c = 0; c < (int)a.grid[1].size(); ++c)
    {
        if (a.grid[1][c] == ':')
            minableCols.push_back(c);
    }

    if (a.grid[1][s_col] == ':')
        a.grid[1][s_col] = 'X';

    for (int c : minableCols)
    {
        if (a.grid[2][c] == ':')
            a.grid[2][c] = 'X';
    }

}

void solve_asteroid(Asteroid &a)
{
    auto [s_row, s_col] = find_outpost(a);
    if (s_row != 0 || s_col < 0) return;

    if (a.w == 3)
    {
        dig_vertical_highway(a, s_col);
    }
    else if (a.h == 3)
    {
        dig_horizontal_highway(a, s_col);
    }
    else { }
}

void write_asteroid(const Asteroid &a, std::ostream &out, bool add_trailing_blank_line = true)
{
    for (const auto &row : a.grid)
    {
        out << row << '\n';
    }
    if (add_trailing_blank_line)
    {
        out << '\n';
    }
}

int main() {
    std::ifstream infile("input.in");
    std::ofstream outfile("output.out");

    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N;
    if (!(infile >> N))
    {
        return 0;
    }

    for (int i = 0; i < N; ++i)
    {
        Asteroid a = read_asteroid(infile);
        solve_asteroid(a);

        bool add_blank = (i + 1 < N);
        write_asteroid(a, outfile, add_blank);
    }

    return 0;
}