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


auto simulateFlight(double minHeight, int timeLimit, const Params& params = defaultParams()) {
    auto accelerations = vector<int>{};
    double height = 0.0;
    double velocity = 0.0;
    bool reachedMinHeight = false;

    for (auto tick = 0; tick < timeLimit; tick++) {
        if (height >= minHeight) {
            reachedMinHeight = true;
        }

        if (height <= 0 && reachedMinHeight) {
            break;
        }

        auto accel = int{};

        if (!reachedMinHeight) {
            accel = computeAscentAccel(height, velocity, minHeight, params);
        } else {
            accel = computeDescentAccel(height, velocity, params);
        }

        accel = max(params.minAccel, min(params.maxAccel, accel));
        accelerations.push_back(accel);

        velocity += accel - GRAVITY;
        height += velocity;

        if (height < 0 && !reachedMinHeight) {
            break;
        }
    }

    return accelerations;
}

int main() {
    ifstream fin("level3.in");
    ofstream fout("level3.out");

    int N, timeLimit;
    fin >> N >> timeLimit;

    for (int i = 0; i < N; i++) {
        int minHeight;
        fin >> minHeight;

        auto flight = simulateFlight(minHeight, timeLimit);

        for (size_t j = 0; j < flight.size(); j++) {
            if (j > 0) fout << " ";
            fout << flight[j];
        }
        fout << "\n";
    }

    fin.close();
    fout.close();

    return 0;
}
