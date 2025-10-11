#Read from file
with open('level3.out') as f:
    line = f.readline().strip()
    accels = list(map(int, line.split()))

min_height = 449
GRAVITY = 10

h, v, max_h = 0, 0, 0
print(f"Total ticks: {len(accels)}")
for i, a in enumerate(accels):
    v += a - GRAVITY
    h += v
    max_h = max(max_h, h)
    if i >= len(accels) - 10:
        print(f"t={i}: a={a}, v={v}, h={h}")

print(f"\nMax height: {max_h}")
print(f"Min height required: {min_height}")
print(f"Reached min: {max_h >= min_height}")
print(f"Final height: {h}")
print(f"Final velocity: {v}")
print(f"Soft landing: {v in [0, -1] and h <= 0}")
