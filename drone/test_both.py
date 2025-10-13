GRAVITY = 10

for level in ["level3", "level4"]:
    with open(f"{level}.in") as f:
        n = int(f.readline())
        data = [list(map(float, f.readline().split())) for _ in range(n)]
    with open(f"{level}.out") as f:
        flights = [
            [list(map(int, p.split(","))) for p in line.strip().split()] for line in f
        ]

    print(f"Testing {level}:")
    print("=" * 80)
    all_ok = True

    for i, (d, acc) in enumerate(zip(data, flights)):
        h, v, x, vx, maxh, reached = 0, 0, 0, 0, 0, False
        errors = []

        for tick, (ax, ay) in enumerate(acc):
            total = abs(ax) + abs(ay)
            if total > 20:
                errors.append(f"tick {tick}: total accel {total} > 20")
            if ay < 0:
                errors.append(f"tick {tick}: ay={ay} < 0")
            if tick == 0 and ay == 0:
                errors.append("no takeoff")

            vx += ax
            v += ay - GRAVITY
            x += vx
            h += v

            if h >= d[1]:
                reached = True
            maxh = max(maxh, h)

            if h <= 0 and reached:
                break

        if not reached:
            errors.append(f"didn't reach min height {d[1]}")
        if vx != 0:
            errors.append(f"vx={vx} (must be 0)")
        if v not in [0, -1]:
            errors.append(f"vy={v} (must be 0 or -1)")
        if abs(x - d[0]) >= 1.0:
            errors.append(f"missed pad by {abs(x - d[0]):.1f}")
        if len(acc) > d[2]:
            errors.append(f"exceeded time limit {len(acc)} > {d[2]}")

        ok = len(errors) == 0
        if not ok:
            all_ok = False

        stat = "PASS" if ok else "FAIL"
        print(
            f"  {stat} Flight {i + 1}: x={x:.0f} (target={d[0]:.0f}), vx={vx}, vy={v}"
        )

        if errors:
            for err in errors:
                print(f"       ERROR: {err}")

    result = "ALL PASS" if all_ok else "SOME FAIL"
    print(f"\n  {level} Result: {result}")
    print()
