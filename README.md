# 🤖 Cloudflight Coding Contest — *Autonomous Drone*

This repository contains my solution for the **Cloudflight Coding Contest (CCC)** problem **“Autonomous Drone”**.  
The challenge simulates the flight of a drone through multiple levels, each adding new mechanics — from basic upward motion to 2D flight and obstacle avoidance.

---

## 🚀 Overview

In *Autonomous Drone*, your goal is to simulate and control a drone’s flight under physical and logical constraints:
- Update position and velocity over discrete time ticks
- Account for gravity and limited acceleration
- Perform a soft landing under specific conditions
- Avoid obstacles and respect time constraints

Each level builds upon the previous one, gradually introducing new dimensions and challenges.

---

## 🧩 Level 2 — Velocity from Acceleration

The drone’s **vertical motion** is simulated based on a sequence of accelerations.

### Rules
- The drone always accelerates **upward** (`acceleration ≥ 0`).
- **Gravity:** 10 velocity units per tick.
- Every tick:
  1. Update velocity → `velocity = velocity + acceleration - gravity`
  2. Update height → `height = height + velocity`
- The height will **never be negative**.
- For each flight, output the drone’s **final height**.

---

## 🧩 Level 3 — Controlled Flight and Soft Landing

Now you control the drone by generating the **sequence of accelerations**.

### Rules
- A **target height** is given.
- The drone must:
  - **Take off** at the first tick.
  - Reach **at least** the target height before landing.
  - **Land softly**, meaning `velocity ∈ {-1, 0}` when height = 0.
- Acceleration range: `0 ≤ acceleration ≤ 20`.
- The flight must end after landing.
- The total flight duration must **not exceed** the given time limit.

---

## 🧩 Level 4 — Two-Dimensional Flight

The drone can now move in both **x** and **y** directions.

### Rules
- **x acceleration:**  
  - Positive → move **right**  
  - Negative → move **left**
- **y acceleration:**  
  - Positive → move **up**
  - Affected by gravity
- Gravity affects **only y**.
- Total acceleration constraint:  
  `0 ≤ |ax| + |ay| ≤ 20`
- The **landing pad** is placed on the ground (not at x = 0).
- Start position: `(x=0, y=0)`
- Must reach a **minimum flight height** during the flight.
- Land **softly** on the landing pad.

---

## 🧩 Level 5 — Buildings and Obstacles

The final level adds **obstacles** — rectangular buildings that the drone must avoid.

### Rules
- **Buildings:** Defined by width and height.
- No building exists at `(x=0, y=0)`.
- The **landing pad** may be on:
  - The ground, or  
  - The **top of a building** (`y = height of that building`)
- The drone must **not collide** with any building.
- Land **softly** on the landing pad.

---

## ✅ Valid Flight Checklist (Level 5)

A flight is considered **valid** if it meets all of the following conditions:

- The drone’s **total acceleration** (`|ax| + |ay|`) is between **0 and 20**.  
- **y acceleration** must **never be negative**.  
- The drone must **take off at the first tick**.  
- The drone must **never touch**, **enter**, or **pass through** a building — except when **landing** on the landing pad.  
- The drone’s **x velocity** must be **0 when landing**.  
- The drone’s **y velocity** must be **-1 or 0 when landing**.  
- After takeoff, the drone may **only touch**:
  - The **ground**, or  
  - The **top of a building**,  
  and **only** when landing on the landing pad.  
- The **acceleration sequence** must **end immediately** after landing.  
- The flight must **not exceed** the given **time limit**.

---
