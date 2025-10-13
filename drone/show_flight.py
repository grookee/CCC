GRAVITY = 10

with open("level3.in") as f:
    n = int(f.readline())
    line = f.readline().strip().split()
    landing_pad_x = float(line[0])
    min_height = int(line[1])
    time_limit = int(line[2])

with open("level3.out") as f:
    line = f.readline().strip()
    accels = []
    for pair in line.split():
        ax, ay = map(int, pair.split(","))
        accels.append((ax, ay))

print(f"Flight 1 Simulation")
print(
    f"Target: landing_pad_x={landing_pad_x}, min_height={min_height}, time_limit={time_limit}"
)
print(f"=" * 80)
print()
print(
    f"{'Tick':>4} {'Ax':>4} {'Ay':>4} {'Vx':>6} {'Vy':>6} {'X':>8} {'Y':>8} {'Status'}"
)
print("-" * 80)

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

    if h >= min_height:
        reached_min = True

    # Determine status
    status = ""
    if not reached_min:
        status = "Ascending"
    elif h > 0:
        status = "Descending"
    else:
        status = "LANDED"

    print(f"{tick:4d} {ax:4d} {ay:4d} {vx:6.1f} {v:6.1f} {x:8.1f} {h:8.1f} {status}")

    if h <= 0 and reached_min:
        break

print("-" * 80)
print()
print(f"Summary:")
print(f"  Maximum height reached: {max_h:.1f} (need {min_height})")
print(f"  Final X position: {x:.1f} (target {landing_pad_x})")
print(f"  Final Y position: {h:.1f}")
print(f"  Final Y velocity: {v:.1f} (need 0 or -1)")
print(f"  Total ticks: {tick + 1} (limit {time_limit})")
print()

if tick > 20:
    print("Last 20 ticks analysis:")
    h, v = 0.0, 0.0
    history = []

    for t, (ax, ay) in enumerate(accels):
        v += ay - GRAVITY
        h += v
        history.append((t, ay, h, v))
        if h <= 0 and t > 0:
            break

    # Show last 20 entries
    start = max(0, len(history) - 20)
    print(f"{'Tick':>4} {'Ay':>4} {'Height':>8} {'Velocity':>8} {'Accel Change':>12}")
    print("-" * 50)

    for i in range(start, len(history)):
        t, ay, height, velocity = history[i]
        accel_change = ""
        if i > start:
            prev_ay = history[i - 1][1]
            change = ay - prev_ay
            if change != 0:
                accel_change = f"{change:+d}"

        print(f"{t:4d} {ay:4d} {height:8.1f} {velocity:8.1f} {accel_change:>12}")

    if len(history) > 5:
        last_accels = [
            history[i][1] for i in range(max(0, len(history) - 10), len(history))
        ]
        changes = [
            last_accels[i + 1] - last_accels[i] for i in range(len(last_accels) - 1)
        ]

        sign_changes = 0
        for i in range(len(changes) - 1):
            if changes[i] * changes[i + 1] < 0:  # opposite signs
                sign_changes += 1

        print()
        if sign_changes > 3:
            print(
                f"⚠ WARNING: Detected {sign_changes} acceleration direction changes in last 10 ticks"
            )
            print("  This indicates oscillation!")
        else:
            print(
                f"✓ STABLE: Only {sign_changes} acceleration direction changes in last 10 ticks"
            )
            print("  No oscillation detected!")
