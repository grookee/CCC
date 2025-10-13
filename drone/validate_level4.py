GRAVITY = 10
MAX_ACCEL = 20

with open("level4.in") as f:
    n = int(f.readline())
    flights_data = []
    for _ in range(n):
        line = f.readline().strip().split()
        landing_pad_x = float(line[0])
        min_height = int(line[1])
        time_limit = int(line[2])
        flights_data.append((landing_pad_x, min_height, time_limit))

with open("level4.out") as f:
    flights = []
    for line in f:
        accels = []
        for pair in line.strip().split():
            ax, ay = map(int, pair.split(","))
            accels.append((ax, ay))
        flights.append(accels)

print(f"Validating {n} flights from level4...")
print("=" * 80)

all_valid = True
for i, ((landing_pad_x, min_h, time_limit), accels) in enumerate(
    zip(flights_data, flights)
):
    h, v = 0.0, 0.0
    x, vx = 0.0, 0.0
    max_h = 0
    reached_min = False
    errors = []

    for tick, (ax, ay) in enumerate(accels):
        total = abs(ax) + abs(ay)
        if total > MAX_ACCEL:
            errors.append(f"Tick {tick}: |ax|+|ay|={total} > 20")
        if ay < 0:
            errors.append(f"Tick {tick}: ay={ay} < 0")
        if tick == 0 and ay == 0:
            errors.append(f"Tick 0: Must take off (ay=0)")

        vx += ax
        v += ay - GRAVITY
        x += vx
        h += v
        max_h = max(max_h, h)

        if h >= min_h:
            reached_min = True

        if h <= 0 and tick > 0:
            if not reached_min:
                errors.append(f"Landed without reaching min_height {min_h}")
            if vx != 0:
                errors.append(f"Landed with vx={vx:.1f} (must be 0)")
            if v not in [0, -1]:
                errors.append(f"Landed with vy={v:.1f} (must be 0 or -1)")
            if abs(x - landing_pad_x) >= 1.0:
                errors.append(
                    f"Missed pad: x={x:.1f}, target={landing_pad_x:.1f}, diff={abs(x - landing_pad_x):.1f}"
                )
            break

    if len(accels) > time_limit:
        errors.append(f"Exceeded time limit: {len(accels)} > {time_limit}")

    if errors:
        all_valid = False
        print(f"✗ Flight {i + 1}: FAILED")
        print(f"  Target: x={landing_pad_x:.1f}, minH={min_h}, timeLimit={time_limit}")
        print(
            f"  Result: x={x:.1f}, maxH={max_h:.1f}, h={h:.1f}, vx={vx:.1f}, vy={v:.1f}, ticks={len(accels)}"
        )
        for error in errors:
            print(f"    ERROR: {error}")
    else:
        print(f"✓ Flight {i + 1}: PASSED")
        print(
            f"  x={x:.1f}, maxH={max_h:.1f}, vx={vx:.1f}, vy={v:.1f}, ticks={len(accels)}/{time_limit}"
        )

print("=" * 80)
if all_valid:
    print("✓ ALL FLIGHTS VALID")
else:
    print("✗ SOME FLIGHTS FAILED")
