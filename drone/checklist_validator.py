GRAVITY = 10
MAX_ACCEL = 20


def validate_flight_checklist(
    flight_num, landing_pad_x, min_height, time_limit, accels
):
    """Validate flight against the complete checklist of requirements."""

    checks = {
        "total_accel_valid": True,
        "y_accel_non_negative": True,
        "takes_off_first_tick": True,
        "x_velocity_zero_at_landing": True,
        "y_velocity_valid_at_landing": True,
        "reaches_min_height": False,
        "no_ground_touch_before_landing": True,
        "sequence_ends_after_landing": True,
        "within_time_limit": True,
        "lands_on_pad": False,
    }

    errors = []
    warnings = []

    h, v = 0.0, 0.0
    x, vx = 0.0, 0.0
    max_h = 0
    reached_min = False
    landed = False
    landing_tick = -1

    for tick, (ax, ay) in enumerate(accels):
        total = abs(ax) + abs(ay)
        if total > MAX_ACCEL or total < 0:
            checks["total_accel_valid"] = False
            errors.append(f"Tick {tick}: |ax|+|ay|={total}, must be in [0, 20]")

        if ay < 0:
            checks["y_accel_non_negative"] = False
            errors.append(f"Tick {tick}: ay={ay}, must be >= 0")

        if tick == 0 and ay == 0:
            checks["takes_off_first_tick"] = False
            errors.append("First tick: ay=0, drone must take off immediately (ay > 0)")

        vx += ax
        v += ay - GRAVITY

        x += vx
        h += v

        if h > max_h:
            max_h = h

        if h >= min_height:
            reached_min = True
            checks["reaches_min_height"] = True

        if h <= 0 and tick > 0:
            landed = True
            landing_tick = tick

            if not reached_min:
                checks["no_ground_touch_before_landing"] = False
                errors.append(
                    f"Tick {tick}: touched ground at h={h:.1f} before reaching min_height={min_height}"
                )

            if vx != 0:
                checks["x_velocity_zero_at_landing"] = False
                errors.append(f"Landing tick {tick}: vx={vx:.1f}, must be 0")

            if v not in [0, -1]:
                checks["y_velocity_valid_at_landing"] = False
                errors.append(f"Landing tick {tick}: vy={v:.1f}, must be 0 or -1")

            if abs(x - landing_pad_x) < 1.0:
                checks["lands_on_pad"] = True
            else:
                errors.append(
                    f"Landing tick {tick}: x={x:.1f}, target={landing_pad_x:.1f}, missed by {abs(x - landing_pad_x):.1f}"
                )

            break

    if landed:
        if landing_tick < len(accels) - 1:
            checks["sequence_ends_after_landing"] = False
            warnings.append(
                f"Sequence continues for {len(accels) - landing_tick - 1} ticks after landing"
            )
    else:
        checks["sequence_ends_after_landing"] = False
        errors.append(f"Never landed (final height: {h:.1f})")

    if len(accels) > time_limit:
        checks["within_time_limit"] = False
        errors.append(f"Exceeded time limit: {len(accels)} ticks > {time_limit} limit")

    all_passed = all(checks.values())

    return {
        "passed": all_passed,
        "checks": checks,
        "errors": errors,
        "warnings": warnings,
        "stats": {
            "final_x": x,
            "final_h": h,
            "final_vx": vx,
            "final_v": v,
            "max_h": max_h,
            "ticks": len(accels),
            "reached_min": reached_min,
            "landed": landed,
        },
    }


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

print("=" * 80)
print("VALIDATING AGAINST COMPLETE CHECKLIST")
print("=" * 80)
print()

all_passed = True
for i, ((landing_pad_x, min_h, time_limit), accels) in enumerate(
    zip(flights_data, flights)
):
    result = validate_flight_checklist(i + 1, landing_pad_x, min_h, time_limit, accels)

    if result["passed"]:
        print(f"✓ Flight {i + 1}: ALL CHECKS PASSED")
    else:
        all_passed = False
        print(f"✗ Flight {i + 1}: FAILED")
        print(f"  Target: x={landing_pad_x:.1f}, minH={min_h}, timeLimit={time_limit}")
        stats = result["stats"]
        print(
            f"  Result: x={stats['final_x']:.1f}, maxH={stats['max_h']:.1f}, "
            + f"finalH={stats['final_h']:.1f}, vx={stats['final_vx']:.1f}, vy={stats['final_v']:.1f}, ticks={stats['ticks']}"
        )
        print()
        print("  Checklist Results:")
        for check_name, passed in result["checks"].items():
            status = "✓" if passed else "✗"
            readable_name = check_name.replace("_", " ").title()
            print(f"    {status} {readable_name}")

        if result["errors"]:
            print()
            print("  Errors:")
            for error in result["errors"]:
                print(f"    • {error}")

        if result["warnings"]:
            print()
            print("  Warnings:")
            for warning in result["warnings"]:
                print(f"    • {warning}")

    print()

print("=" * 80)
print()
print("CHECKLIST SUMMARY:")
print("  1. Total acceleration (|ax| + |ay|) between 0 and 20: ", end="")
print("✓" if all_passed else "Check individual results")
print("  2. Y acceleration must not be negative: ", end="")
print("✓" if all_passed else "Check individual results")
print("  3. Drone must take off at first tick: ", end="")
print("✓" if all_passed else "Check individual results")
print("  4. X velocity must be 0 when landing: ", end="")
print("✓" if all_passed else "Check individual results")
print("  5. Y velocity must be -1 or 0 when landing: ", end="")
print("✓" if all_passed else "Check individual results")
print("  6. Must reach minimum height at least once: ", end="")
print("✓" if all_passed else "Check individual results")
print("  7. No ground touch before landing on pad: ", end="")
print("✓" if all_passed else "Check individual results")
print("  8. Sequence ends after landing: ", end="")
print("✓" if all_passed else "Check individual results")
print("  9. Within time limit: ", end="")
print("✓" if all_passed else "Check individual results")
print()
print("=" * 80)

if all_passed:
    print("✓✓✓ ALL FLIGHTS PASS ALL CHECKLIST REQUIREMENTS ✓✓✓")
else:
    print("✗✗✗ SOME FLIGHTS FAIL CHECKLIST REQUIREMENTS ✗✗✗")

print("=" * 80)
