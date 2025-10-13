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

print(f"Verifying {n} flights...")
print()

all_success = True
for i, ((landing_pad_x, min_h, time_limit), accels) in enumerate(
    zip(flights_data, flights)
):
    h, v, max_h = 0, 0, 0
    for ax, ay in accels:
        v += ay - GRAVITY
        h += v
        max_h = max(max_h, h)
        if h <= 0 and max_h >= min_h:
            break

    x, vx = 0, 0
    for ax, ay in accels:
        vx += ax
        x += vx

    reached_min = max_h >= min_h
    landed = h <= 0
    soft_landing = v in [0, -1]
    within_time = len(accels) <= time_limit
    on_pad = abs(x - landing_pad_x) < 1.0

    success = reached_min and landed and soft_landing and within_time and on_pad
    all_success = all_success and success

    status = "✓" if success else "✗"
    print(f"{status} Flight {i + 1}:")
    print(f"   Target: x={landing_pad_x:7.1f}, minH={min_h:4d}, time≤{time_limit:4d}")
    print(
        f"   Result: x={x:7.1f}, maxH={max_h:4d}, finalH={h:4d}, finalV={v:3d}, ticks={len(accels):3d}"
    )

    if not success:
        if not reached_min:
            print(f"   ERROR: Did not reach minimum height (got {max_h}, need {min_h})")
        if not landed:
            print(f"   ERROR: Did not land (final height {h})")
        if not soft_landing:
            print(f"   ERROR: Not a soft landing (velocity {v}, need 0 or -1)")
        if not within_time:
            print(f"   ERROR: Exceeded time limit ({len(accels)} > {time_limit})")
        if not on_pad:
            print(f"   ERROR: Missed landing pad (x={x:.1f}, target={landing_pad_x})")
    print()

print()
if all_success:
    print("✓ All flights successful!")
else:
    print("✗ Some flights failed")
