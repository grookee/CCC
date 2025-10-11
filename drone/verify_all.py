# Verify all flights
GRAVITY = 10

with open('level3.in') as f:
    n = int(f.readline())
    time_limit = int(f.readline())
    min_heights = [int(f.readline()) for _ in range(n)]

with open('level3.out') as f:
    flights = [list(map(int, line.strip().split())) for line in f]

for i, (min_h, accels) in enumerate(zip(min_heights, flights)):
    h, v, max_h = 0, 0, 0
    for a in accels:
        v += a - GRAVITY
        h += v
        max_h = max(max_h, h)
    
    success = (max_h >= min_h) and (h <= 0) and (v in [0, -1]) and (len(accels) <= time_limit)
    status = "✓" if success else "✗"
    print(f"{status} Flight {i+1}: minH={min_h:4d}, maxH={max_h:4d}, finalH={h:3d}, finalV={v:3d}, ticks={len(accels):3d}")

