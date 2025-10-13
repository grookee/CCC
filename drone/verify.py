accels = [20, 20, 20, 20, 20, 20, 20, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 9, 9, 9, 8, 7, 2]
min_height = 449
GRAVITY = 10

h, v, max_h = 0, 0, 0
for i, a in enumerate(accels):
    v += a - GRAVITY
    h += v
    max_h = max(max_h, h)
    if i < 20 or i >= len(accels) - 5:
        print(f"t={i}: a={a}, v={v}, h={h}")

print(f"\nMax height: {max_h}")
print(f"Min height required: {min_height}")
print(f"Reached min: {max_h >= min_height}")
print(f"Final height: {h}")
print(f"Final velocity: {v}")
print(f"Soft landing: {v in [0, -1] and h <= 0}")
