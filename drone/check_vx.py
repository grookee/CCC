GRAVITY = 10

with open("level3.in") as f:
    n = int(f.readline())
    flights_data = []
    for _ in range(n):
        line = f.readline().strip().split()
        landing_pad_x = float(line[0])
        min_height = int(line[1])
        time_limit = int(line[2])
        flights_data.append((landing_pad_x, min_height, time_limit))

with open("level3.out") as f:
    flights = []
    for line in f:
        accels = []
        for pair in line.strip().split():
            ax, ay = map(int, pair.split(","))
            accels.append((ax, ay))
        flights.append(accels)

print(f"Checking X velocity at landing for {n} flights...")
print("=" * 80)

all_good = True
for i, ((landing_pad_x, min_h, time_limit), accels) in enumerate(
    zip(flights_data, flights)
):
    h, v = 0.0, 0.0
    x, vx = 0.0, 0.0
    max_h = 0
    reached_min = False

    for tick, (ax, ay) in enumerate(accels):
        vx += ax
        v += ay - GRAVITY
        x += vx
        h += v
        max_h = max(max_h, h)

        if h >= min_h:
            reached_min = True

        if h <= 0 and reached_min and tick > 0:
            break

    status = "✓" if vx == 0 else "✗"
    if vx != 0:
        all_good = False

    print(
        f"{status} Flight {i + 1}: x={x:8.1f} (target={landing_pad_x:8.1f}), vx={vx:6.1f} (must be 0), vy={v:3.1f}"
    )

print("=" * 80)
if all_good:
    print("✓ All flights have vx=0 at landing")
else:
    print("✗ Some flights have non-zero vx at landing - THIS IS THE PROBLEM!")
