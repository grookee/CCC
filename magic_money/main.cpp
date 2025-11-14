#include <bits/stdc++.h>

class Input {
public:
    int N;
    int C;
    std::vector<std::vector<int>> currency;
};

std::vector<int> solve(int N, int C, const std::vector<std::vector<int>>& currency) {
    std::vector<int> results;
    for (int i = 0; i < N; i++)
    {
        int canMake = 1;
        for (int coin : currency[i])
        {
            if (coin == canMake)
            {
                canMake++;
            } else if (coin > canMake)
            {
                break;
            }
        }

        results.push_back(canMake);
    }
    return results;
}
int main() {
    std::ifstream fin("input.in");
    std::ofstream fout("output.out");

    int N, C;
    fin >> N >> C;
    std::vector<std::vector<int>> currency(N);
    for (int i = 0; i < N; i++) {
        currency[i].resize(C);
        for (int j = 0; j < C; j++) {
            fin >> currency[i][j];
        }
    }

    std::vector<int> res = solve(N, C, currency);
    for (int v : res) {
        fout << v << "\n";
    }

    return 0;
}