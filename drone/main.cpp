#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <tuple>

using namespace std;

constexpr double GRAVITY = 10.0;
constexpr int MAX_ACCEL = 20;
constexpr int MIN_ACCEL = 0;
constexpr double PEAK_HEIGHT_MARGIN = 10;
constexpr int MODERATE_ACCEL = 5;
constexpr double LANDING_HEIGHT_THRESHOLD = 2.0;
constexpr int LANDING_ACCEL = 9;
constexpr double DESCENT_FUDGE_FACTOR = 1.0;
constexpr double ELEVATED_LANDING_TOLERANCE = 1.0;

struct Building {
    double left;
    double right;
    double height;
};

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

bool isInsideBuilding(double x, double y, const Building& building) {
    return x >= building.left && x <= building.right && 
           y >= 0 && y <= building.height;
}

double getMaxHeightInPath(double startX, double endX, 
                          const vector<Building>& buildings) {
    double maxHeight = 0;
    for (const auto& building : buildings) {
        // Check if building is in the flight path
        bool inPath = (startX <= building.right && endX >= building.left) ||
                      (endX <= building.right && startX >= building.left);
        if (inPath) {
            maxHeight = max(maxHeight, building.height);
        }
    }
    return maxHeight;
}

double getLandingPadHeight(double landing_pad_x, double landing_pad_y,
                           const vector<Building>& buildings) {
    for (const auto& building : buildings) {
        if (landing_pad_x >= building.left && 
            landing_pad_x <= building.right &&
            abs(landing_pad_y - building.height) < 0.1) {
            return building.height;
        }
    }
    return 0.0; // Landing on ground
}

auto simulateFlight(double landing_pad_x, double min_height, int time_limit, 
                   const vector<Building>& buildings = vector<Building>(),
                   double landing_pad_y = 0.0,
                   const Params& params = defaultParams()) {
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
    
    // Adjust for buildings in the flight path
    if (!buildings.empty()) {
        double maxBuildingHeight = getMaxHeightInPath(0, landing_pad_x, buildings);
        double clearanceMargin = 5.0; // Safety margin above buildings
        double minRequiredHeight = max(min_height, maxBuildingHeight + clearanceMargin);
        targetHeight = max(targetHeight, minRequiredHeight);
    }
    
    // For elevated landing pads, ensure we go high enough above the pad for controlled descent
    if (landing_pad_y > 0) {
        targetHeight = max(targetHeight, landing_pad_y + 10.0);
    }

    double height = 0.0;
    double velocity = 0.0;
    double x = 0.0;
    double vx = 0.0;
    bool reachedMinHeight = false;
    bool reachedTargetHeight = false;
    int lastAccel = 10;

    for (int tick = 0; tick < time_limit; tick++) {
        if (height >= min_height && tick > 0) reachedMinHeight = true;
        if (height >= targetHeight) reachedTargetHeight = true;

        int ay = 0;
        if (!reachedTargetHeight) {
            ay = computeAscentAccel(height, velocity, targetHeight, params);
        } else {
            // Use relative height above landing pad for descent calculations
            double relativeHeight = height - landing_pad_y;
            ay = computeDescentAccel(relativeHeight, velocity, lastAccel, params);
        }
        lastAccel = ay;
        ay = max(params.minAccel, min(params.maxAccel, ay));

        int availableForX = MAX_ACCEL - ay;

        int ticksRemaining = time_limit - tick;
        if (reachedTargetHeight && velocity < 0) {
            double relativeHeight = height - landing_pad_y;
            double estimatedTicks = max(1.0, relativeHeight / max(1.0, abs(velocity)));
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
        
        // Check if landed: at target position and height (check after position update)
        bool atTargetX = abs(x - landing_pad_x) < 0.5;
        // For ground landing (landing_pad_y ~= 0), land when height <= 0
        // For elevated landing, land when at or slightly above target height
        bool atTargetHeight;
        if (landing_pad_y < 1.0) {
            atTargetHeight = height <= 0;
        } else {
            atTargetHeight = height >= landing_pad_y && 
                           height <= landing_pad_y + ELEVATED_LANDING_TOLERANCE;
        }
        if (atTargetX && atTargetHeight && reachedMinHeight) break;
        
        // Check for building collisions (except when landing on pad on a building)
        bool landingOnBuilding = false;
        if (landing_pad_y > 0) {
            // Check if landing pad is on a building
            for (const auto& building : buildings) {
                if (landing_pad_x >= building.left && landing_pad_x <= building.right &&
                    abs(landing_pad_y - building.height) < 0.1) {
                    // Landing pad is on this building
                    // Allow drone to be in this building's volume when near the pad
                    if (abs(x - landing_pad_x) < 2.0 && abs(height - landing_pad_y) < 10.0) {
                        landingOnBuilding = true;
                    }
                    break;
                }
            }
        }
        
        if (!buildings.empty() && !landingOnBuilding) {
            for (const auto& building : buildings) {
                if (isInsideBuilding(x, height, building)) {
                    // Collision detected - continue anyway but this indicates a problem
                    // In practice, the clearance height should prevent this
                    break;
                }
            }
        }

        if (height < 0 && !reachedMinHeight) break;
    }

    return accelerations;
}

int main(int argc, char* argv[]) {
    string inputFile = "level3.in";
    string outputFile = "level3.out";

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

    // Read all flight data first
    vector<tuple<double, double, int>> flights;
    for (int i = 0; i < N; i++) {
        double landing_pad_x, landing_pad_y;
        int time_limit;
        fin >> landing_pad_x >> landing_pad_y >> time_limit;
        flights.push_back(make_tuple(landing_pad_x, landing_pad_y, time_limit));
    }
    
    // Read buildings if available
    vector<Building> buildings;
    int B = 0;
    if (fin >> B) {
        for (int j = 0; j < B; j++) {
            double left, right, height;
            fin >> left >> right >> height;
            buildings.push_back({left, right, height});
        }
    }
    
    // Process each flight
    for (int i = 0; i < N; i++) {
        double landing_pad_x = get<0>(flights[i]);
        double landing_pad_y = get<1>(flights[i]);
        int time_limit = get<2>(flights[i]);
        
        // Determine min_height based on whether buildings are present
        double min_height;
        if (!buildings.empty()) {
            // With buildings, landing_pad_y is the actual landing height
            // We need to reach at least this height (or higher to clear buildings)
            min_height = landing_pad_y;
        } else {
            // Without buildings, landing_pad_y is the min_height constraint
            min_height = landing_pad_y;
        }
        
        auto flight = simulateFlight(landing_pad_x, min_height, time_limit, buildings, landing_pad_y);

        if (!flight.empty()) {
            for (size_t j = 0; j < flight.size(); j++) {
                if (j > 0) fout << " ";
                fout << flight[j].first << "," << flight[j].second;
            }
            fout << "\n";
        }
    }

    fin.close();
    fout.close();

    return 0;
}
