GRAVITY = 10
MAX_ACCEL = 20


def validate_flight(flight_num, landing_pad_x, min_height, time_limit, accels):
    """Strictly validate a single flight according to all rules."""
    errors = []
    warnings = []

    h, v = 0.0, 0.0
    x, vx = 0.0, 0.0
    max_h = 0
    reached_min = False

    for tick, (ax, ay) in enumerate(accels):
        if ay < 0 or ay > MAX_ACCEL:
            errors.append(f"Tick {tick}: ay={ay} out of range [0, 20]")

        if ax < -MAX_ACCEL or ax > MAX_ACCEL:
            errors.append(f"Tick {tick}: ax={ax} out of range [-20, 20]")

        total = abs(ax) + abs(ay)
        if total > MAX_ACCEL:
            errors.append(f"Tick {tick}: |ax|+|ay|={total} exceeds 20")

        vx += ax
        v += ay - GRAVITY

        x += vx
        h += v

        if h > max_h:
            max_h = h

        if h >= min_height:
            reached_min = True

        if h <= 0 and tick > 0:
            if not reached_min:
                errors.append(
                    f"Landed at tick {tick} without reaching min_height {min_height} (max was {max_h})"
                )

            if v not in [0, -1]:
                errors.append(f"Landed with velocity {v}, must be 0 or -1")

            if abs(x - landing_pad_x) >= 1.0:
                errors.append(
                    f"Missed landing pad: x={x:.1f}, target={landing_pad_x:.1f}, diff={abs(x - landing_pad_x):.1f}"
                )

            break
    else:
        if h > 0:
            errors.append(
                f"Did not land within {len(accels)} ticks (final height {h:.1f})"
            )
        elif h < 0:
            errors.append(f"Crashed through ground (final height {h:.1f})")

    if len(accels) > time_limit:
        errors.append(f"Exceeded time limit: {len(accels)} > {time_limit}")

    if len(accels) > 0 and accels[0][1] == 0:
        warnings.append("Did not take off immediately (first ay=0)")

    return (
        errors,
        warnings,
        {
            "final_x": x,
            "final_h": h,
            "final_vx": vx,
            "final_v": v,
            "max_h": max_h,
            "ticks": len(accels),
            "reached_min": reached_min,
        },
    )


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
            parts = pair.split(",")
            if len(parts) != 2:
                print(f"ERROR: Invalid format in output: '{pair}'")
                continue
            ax, ay = map(int, parts)
            accels.append((ax, ay))
        flights.append(accels)

print(f"Validating {n} flights...")
print("=" * 80)
print()

all_valid = True
for i, ((landing_pad_x, min_h, time_limit), accels) in enumerate(
    zip(flights_data, flights)
):
    errors, warnings, stats = validate_flight(
        i + 1, landing_pad_x, min_h, time_limit, accels
    )

    if errors:
        all_valid = False
        print(f"✗ Flight {i + 1}: FAILED")
        print(f"  Target: x={landing_pad_x:.1f}, minH={min_h}, timeLimit={time_limit}")
        print(
            f"  Result: x={stats['final_x']:.1f}, maxH={stats['max_h']:.1f}, "
            + f"finalH={stats['final_h']:.1f}, finalV={stats['final_v']:.1f}, ticks={stats['ticks']}"
        )
        for error in errors:
            print(f"  ERROR: {error}")
        for warning in warnings:
            print(f"  WARNING: {warning}")
    else:
        status = "✓"
        print(f"{status} Flight {i + 1}: PASSED")
        if warnings:
            for warning in warnings:
                print(f"  WARNING: {warning}")
    print()

print("=" * 80)
if all_valid:
    print("✓ ALL FLIGHTS VALID")
else:
    print("✗ SOME FLIGHTS HAVE ERRORS")
