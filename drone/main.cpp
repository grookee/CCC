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

int computeDescentAccel(double currentHeight, double velocity, int lastAccel, const Params& params) {
    if (velocity > 0) {
        return params.minAccel;
    }

    if (currentHeight < 1.0) {
        if (velocity >= -1.5) {
            return params.landingAccel;
        }
        return params.maxAccel;
    }

    if (velocity == 0) {
        if (currentHeight <= params.landingTreshold) {
            return params.landingAccel;
        }
        return params.minAccel;
    }

    double vSquared = velocity * velocity;
    double neededDecel = (vSquared - 1.0) / (2.0 * currentHeight);
    double idealAccel = params.gravity + neededDecel;

    if (idealAccel > params.maxAccel) idealAccel = params.maxAccel;
    if (idealAccel < params.minAccel) idealAccel = params.minAccel;

    int targetAccel = static_cast<int>(round(idealAccel));

    if (currentHeight > 3.0 && currentHeight < 100.0 && abs(targetAccel - lastAccel) == 1) {
        if (abs(velocity + 1.0) > 5.0) {
            return targetAccel;
        }
        return lastAccel;
    }

    return targetAccel;
}

int computeXAccel(double currentX, double vx, double targetX, int ticksRemaining, int availableAccel) {
    if (ticksRemaining <= 0) return 0;

    double deltaX = targetX - currentX;

    if (abs(deltaX) < 0.5 && vx == 0) {
        return 0;
    }

    if (vx != 0) {
        int sign = (vx > 0) ? 1 : -1;
        double absVx = abs(vx);

        int maxBrake = min(availableAccel, MAX_ACCEL);
        double brakingDistance;

        if (maxBrake >= absVx) {
            brakingDistance = absVx;
        } else {
            int ticks = (int)ceil(absVx / maxBrake);
            brakingDistance = 0;
            double v = absVx;
            for (int i = 0; i < ticks && v > 0; i++) {
                v -= maxBrake;
                if (v < 0) v = 0;
                brakingDistance += v;
            }
            brakingDistance += absVx;
        }

        double distanceToTarget = abs(deltaX);
        bool shouldBrake = false;

        if (vx > 0) {
            shouldBrake = (currentX + brakingDistance >= targetX - 0.5);
        } else {
            shouldBrake = (currentX - brakingDistance <= targetX + 0.5);
        }

        if (shouldBrake) {
            int brake = min(availableAccel, max(1, (int)ceil(absVx)));
            return -sign * brake;
        }
    }

    int sign = (deltaX > 0) ? 1 : -1;

    if ((deltaX > 0 && vx < 0) || (deltaX < 0 && vx > 0)) {
        int brake = min(availableAccel, MAX_ACCEL);
        return vx > 0 ? -brake : brake;
    }

    double maxAccel = min((double)availableAccel, (double)MAX_ACCEL);

    if (ticksRemaining > 4) {
        double timeToMidpoint = ticksRemaining / 2.0;
        double neededAccel = abs(deltaX) / (timeToMidpoint * timeToMidpoint);
        maxAccel = min(maxAccel, neededAccel + 1.0);
    }

    return sign * min((int)round(maxAccel), availableAccel);
}

auto simulateFlight(double landing_pad_x, double min_height, int time_limit, const Params& params = defaultParams()) {
    vector<pair<int, int>> accelerations;

    double absTargetX = abs(landing_pad_x);

    double avgXAccelDuringDescent = 7.0;

    double estimatedXTime = 2.0 * sqrt(absTargetX / avgXAccelDuringDescent);

    estimatedXTime *= 1.3;

    double avgDescentRate = 10.0;
    double neededHeight = estimatedXTime * avgDescentRate;

    double targetHeight = max((double)min_height, neededHeight);
    double maxReasonableHeight = time_limit * 2.0;
    targetHeight = min(targetHeight, maxReasonableHeight);
    targetHeight = max(targetHeight, min_height * 1.1);

    double height = 0.0;
    double velocity = 0.0;
    double x = 0.0;
    double vx = 0.0;
    bool reachedMinHeight = false;
    bool reachedTargetHeight = false;
    int lastAccel = 10;

    for (int tick = 0; tick < time_limit; tick++) {
        if (height >= min_height) reachedMinHeight = true;
        if (height >= targetHeight) reachedTargetHeight = true;

        if (height <= 0 && reachedMinHeight) break;

        int ay = 0;
        if (!reachedTargetHeight) {
            ay = computeAscentAccel(height, velocity, targetHeight, params);
        } else {
            ay = computeDescentAccel(height, velocity, lastAccel, params);
        }
        lastAccel = ay;
        ay = max(params.minAccel, min(params.maxAccel, ay));

        int availableForX = MAX_ACCEL - ay;

        int ticksRemaining = time_limit - tick;
        if (reachedTargetHeight && velocity < 0) {
            double estimatedTicks = max(1.0, height / max(1.0, abs(velocity)));
            ticksRemaining = min(ticksRemaining, (int)ceil(estimatedTicks) + 5);
        }

        int ax = computeXAccel(x, vx, landing_pad_x, ticksRemaining, availableForX);

        int totalAccel = abs(ax) + abs(ay);
        if (totalAccel > MAX_ACCEL) {
            int excess = totalAccel - MAX_ACCEL;
            if (ax > 0) {
                ax = max(0, ax - excess);
            } else {
                ax = min(0, ax + excess);
            }
        }

        accelerations.push_back({ax, ay});

        vx += ax;
        x += vx;
        velocity += ay - GRAVITY;
        height += velocity;

        if (height < 0 && !reachedMinHeight) break;
    }

    return accelerations;
}

int main(int argc, char* argv[]) {
    string inputFile = "input.in";
    string outputFile = "output.out";

    if (argc >= 2) {
        inputFile = argv[1];
    }
    if (argc >= 3) {
        outputFile = argv[2];
    }

    ifstream fin(inputFile);
    ofstream fout(outputFile);

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
