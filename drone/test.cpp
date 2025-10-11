#include <iostream>
using namespace std;

int main() {
    int h = 0, v = 0;
    int accel[] = {20, 20, 20, 20, 20, 20, 20, 0, 0, 0, 0, 0, 0, 0, 9};
    for (int i = 0; i < 15; i++) {
        int a = accel[i];
        v += a - 10;
        h += v;
        cout << "t=" << i << " a=" << a << " v=" << v << " h=" << h << "\n";
    }
    return 0;
}
