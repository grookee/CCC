GRAVITY = 10

with open("level4.in") as f:
    n = int(f.readline())
    line = f.readline().strip().split()
    landing_pad_x = float(line[0])
    min_height = int(line[1])
    time_limit = int(line[2])

with open("level4.out") as f:
    line = f.readline().strip()
    accels = []
    for pair in line.split():
        ax, ay = map(int, pair.split(","))
        accels.append((ax, ay))

print(f"Flight 1 Debug")
print(f"Target: x={landing_pad_x}, min_height={min_height}, time_limit={time_limit}")
print("=" * 100)
print()
print(
    f"{'Tick':>4} {'Ax':>4} {'Ay':>4} {'Vx':>8} {'Vy':>8} {'X':>10} {'Y':>10} {'Status':<20} {'Notes'}"
)
print("-" * 100)

h, v = 0.0, 0.0
x, vx = 0.0, 0.0
max_h = 0
reached_min = False

for tick, (ax, ay) in enumerate(accels):
    total = abs(ax) + abs(ay)
    notes = ""
    if total > 20:
        notes += f"ACCEL_OVER! "

    vx += ax
    v += ay - GRAVITY

    x += vx
    h += v

    max_h = max(max_h, h)

    if h >= min_height and not reached_min:
        reached_min = True
        notes += "REACHED_MIN! "

    status = ""
    if not reached_min:
        status = "Ascending"
    elif h > 0:
        status = "Descending"
    else:
        status = "LANDED"
        notes += f"Target_X={landing_pad_x}, Diff={abs(x - landing_pad_x):.1f}"

    print(
        f"{tick:4d} {ax:4d} {ay:4d} {vx:8.1f} {v:8.1f} {x:10.1f} {h:10.1f} {status:<20} {notes}"
    )

    if h <= 0 and reached_min:
        break

print("-" * 100)
print()
print(f"Summary:")
print(f"  Target X: {landing_pad_x:.1f}")
print(f"  Final X: {x:.1f}")
print(f"  Difference: {abs(x - landing_pad_x):.1f}")
print(f"  Final Vx: {vx:.1f} (must be 0)")
print(f"  Final Vy: {v:.1f} (must be 0 or -1)")
print(f"  Max height: {max_h:.1f} (need {min_height})")
print(f"  Ticks used: {tick + 1} (limit {time_limit})")
print()

print("PROBLEM ANALYSIS:")
print(
    f"  Min height is very LOW ({min_height}) compared to X distance ({landing_pad_x})"
)
print(
    f"  Drone reached min height too quickly (tick ~{[i for i, (ax, ay) in enumerate(accels) if sum(accels[j][0] for j in range(i + 1)) >= 0][0] if accels else 0})"
)
print(f"  Not enough time to accelerate horizontally")
print(f"  Need to DELAY reaching min height or build X velocity first")
print()
print("SUGGESTION:")
print("  When min_height is low but X distance is large:")
print("  1. Build up horizontal velocity FIRST while ascending")
print("  2. OR overshoot min_height to give more time for X acceleration")
print("  3. Current strategy reaches min_height too fast")
