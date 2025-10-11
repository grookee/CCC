#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>

using namespace std;

const int GRAVITY = 10;

vector<int> simulateFlight(int minHeight, int timeLimit) {
    vector<int> accelerations;
    int height = 0;
    int velocity = 0;
    bool reachedMinHeight = false;

    for (int tick = 0; tick < timeLimit; tick++) {
        if (height >= minHeight) {
            reachedMinHeight = true;
        }

        if (height <= 0 && reachedMinHeight) {
            break;
        }

        int accel;

        if (!reachedMinHeight) {
            int estimatedPeak = height + (velocity * velocity) / 20;

            if (estimatedPeak >= minHeight + 10) {
                accel = 0;
            } else if (estimatedPeak >= minHeight) {
                accel = 5;
            } else {
                accel = 20;
            }
        } else {
            if (velocity > 0) {
                accel = 0;
            } else if (velocity == 0) {
                if (height <= 2) {
                    accel = 9;
                } else {
                    accel = 0;
                }
            } else {
                int v_squared = velocity * velocity;
                double idealAccel = GRAVITY + (double)(v_squared - 1) / (2.0 * height);

                if (idealAccel > 20) {
                    accel = 20;
                } else if (idealAccel >= 0 && idealAccel <= 20) {
                    accel = (int)round(idealAccel);
                } else {
                    accel = 0;
                }
            }
        }

        if (accel < 0) accel = 0;
        if (accel > 20) accel = 20;

        accelerations.push_back(accel);

        velocity += accel - GRAVITY;
        height += velocity;

        if (height <= 0 && reachedMinHeight) {
            break;
        }

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

        vector<int> flight = simulateFlight(minHeight, timeLimit);

        // Output the flight
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
