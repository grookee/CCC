#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>

using namespace std;

constexpr double GRAVITY = 10.0;
constexpr int MAX_ACCEL = 20;
constexpr int MIN_ACCEL = 0;
constexpr double PEAK_HEIGHT_MARGIN = 10;
constexpr int MODERATE_ACCEL = 5;
constexpr double LANDING_HEIGHT_THRESHOLD = 2.0;
constexpr int LANDING_ACCEL = 9;
constexpr double DESCENT_FUDGE_FACTOR = 1.0;

struct Params {
    double gravity;
    int maxAccel;
    int minAccel;
    double peakMargin;
    int moderateAccel;
    double landingTreshold;
    int landingAccel;
    double descentFudge;
};

constexpr Params defaultParams() {
    return {GRAVITY, MAX_ACCEL, MIN_ACCEL, PEAK_HEIGHT_MARGIN, MODERATE_ACCEL, LANDING_HEIGHT_THRESHOLD, LANDING_ACCEL, DESCENT_FUDGE_FACTOR};
}

int computeAscentAccel(double currentHeight, double velocity, double minHeight, const Params& params) {
    double estimatedPeak = currentHeight + (velocity * velocity) / (2.0 * params.gravity);

    if (estimatedPeak >= minHeight + params.peakMargin) {
        return params.minAccel;
    } else if (estimatedPeak >= minHeight) {
        return params.moderateAccel;
    } else {
        return params.maxAccel;
    }
}

int computeDescentAccel(double currentHeight, double velocity, const Params& params) {
    if (velocity > 0) {
        return params.minAccel;
    } else if (velocity == 0) {
        return (currentHeight <= params.landingTreshold) ? params.landingAccel : params.minAccel;
    } else {
        double vSquared = velocity * velocity;
        double idealAccel = params.gravity + (vSquared - params.descentFudge) / (2.0 * currentHeight);
        
        if (idealAccel > params.maxAccel) return params.maxAccel;
        if (idealAccel < params.minAccel) return params.minAccel;

        return static_cast<int>(round(idealAccel));
    }
}

vector<int> computeAxSequence(int t, double d) {
    int maxA = 20;
    vector<int> ax(t, 0);
    if (t <= 0) return ax;
    if (t == 1) {
        ax[0] = 0;
        return ax;
    }
    if (t == 2) {
        int a = round(d / 2.0);
        a = max(-maxA, min(maxA, a));
        ax[0] = a;
        ax[1] = -a;
        return ax;
    }
    if (t == 3) {
        int target = round(d);
        for (int a0 = -maxA; a0 <= maxA; a0++) {
            for (int a1 = -maxA; a1 <= maxA; a1++) {
                int a2 = -a0 - a1;
                if (abs(a2) > maxA) continue;
                if (2 * a0 + a1 == target) {
                    ax[0] = a0;
                    ax[1] = a1;
                    ax[2] = a2;
                    return ax;
                }
            }
        }
        // if not found, use 0
        return ax;
    }
    if (t == 7) {
        double dd = d / 6.0;
        int a = round(dd / 2.0);
        int b = round(dd - 2.0 * a);
        int c = -3 * a - 3 * b;
        a = max(-maxA, min(maxA, a));
        b = max(-maxA, min(maxA, b));
        c = max(-maxA, min(maxA, c));
        ax = {a, a, a, b, b, b, c};
        return ax;
    }
    // for other t, use bang-bang if even
    if (t % 2 == 0) {
        int m = t / 2;
        double a_double = d / (m * m);
        int a = round(a_double);
        a = max(-maxA, min(maxA, a));
        for (int i = 0; i < m; i++) ax[i] = a;
        for (int i = m; i < t; i++) ax[i] = -a;
        return ax;
    }
    // for odd, perhaps similar to t=3 or t=7, but for simplicity, use 0
    return ax;
}

auto simulateFlight(double landing_pad_x, double min_height, int time_limit, const Params& params = defaultParams()) {
    // first, simulate y
    vector<int> ay_sequence;
    double height = 0.0;
    double velocity = 0.0;
    bool reachedMinHeight = false;

    for (int tick = 0; tick < time_limit; tick++) {
        if (height >= min_height) reachedMinHeight = true;

        if (height <= 0 && reachedMinHeight) break;

        int ay = 0;

        if (!reachedMinHeight) {
            ay = computeAscentAccel(height, velocity, min_height, params);
        } else {
            ay = computeDescentAccel(height, velocity, params);
        }

        ay = max(params.minAccel, min(params.maxAccel, ay));
        ay_sequence.push_back(ay);

        velocity += ay - GRAVITY;
        height += velocity;

        if (height < 0 && !reachedMinHeight) break;
    }

    int t = ay_sequence.size();
    auto ax_sequence = computeAxSequence(t, landing_pad_x);

    vector<pair<int, int>> accelerations;
    for (int i = 0; i < t; i++) {
        int ax = ax_sequence[i];
        int ay = ay_sequence[i];
        int total = abs(ax) + abs(ay);
        if (total > 20) {
            int excess = total - 20;
            if (abs(ax) >= excess) {
                ax = ax > 0 ? ax - excess : ax + excess;
            } else {
                ay = ay - excess; // since ay >=0
            }
            ax = max(-20, min(20, ax));
            ay = max(0, min(20, ay));
        }
        accelerations.push_back({ax, ay});
    }

    return accelerations;
}

int main() {
    ifstream fin("level3.in");
    ofstream fout("level3.out");

    int N;
    fin >> N;

    for (int i = 0; i < N; i++) {
        double landing_pad_x, min_height;
        int time_limit;
        fin >> landing_pad_x >> min_height >> time_limit;

        auto flight = simulateFlight(landing_pad_x, min_height, time_limit);

        for (size_t j = 0; j < flight.size(); j++) {
            if (j > 0) fout << " ";
            fout << flight[j].first << "," << flight[j].second;
        }
        fout << "\n";
    }

    fin.close();
    fout.close();

    return 0;
}