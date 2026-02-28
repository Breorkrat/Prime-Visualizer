## Prime Visualizer
A high-performance mathematical tool built in C and Raylib to visualize prime numbers (and multiples) in a polar coordinate system (r = θ = n). 

This project demonstrates the hidden patterns within the distribution of primes, revealing the "arms" and "spirals" that emerge from simple mathematical rules.

![Spiral Preview](screenshots/preview.gif)

## Technical Highlights
- **High Performance:** Uses a Segmented Sieve of Eratosthenes to calculate millions of primes on the fly with minimal memory footprint.
- **Advanced Culling:** Implements Binary Search to find visible primes and viewport culling to maintain high FPS while moving even with millions of points.
- **Precision:** Utilizes 64-bit integers and double-precision floating point coordinates to prevent "jitter" or "drifting" at astronomical distances.

## Controls

### Navigation
- **[WASD]**: Pan the camera
- **[Mouse Wheel]**: Zoom in/out
- **[R]**: Auto-zoom: Fill screen mode
- **[F]**: Auto-zoom: Full spiral view

### The Spiral
- **[Numbers 0-9]**: Type a number and press **[Enter]** to jump to a specific multiple.
- **[Left/Right Arrows]**: Increment/Decrement the current multiple (0 = Primes).
- **[Up/Down Arrows]**: Adjust generation speed (PPS).
- **[Enter]**: Pause / Resume generation.

### Visuals
- **[C]**: Cycle through Color Modes (Static, Breathing, etc.).
- **[TAB]**: Toggle Color Picker (Change custom colors for gradients).
- **[P]**: Save a high-resolution screenshot.
- **[F1/F2/F3]**: Toggle UI overlays (Controls, FPS, Stats).

## 🛠 Installation & Build

### Requirements
- A C compiler (GCC/MinGW)
- [Raylib](https://www.raylib.com/)

### Build
`make #Which will create the executable as ./bin/Debug/Prime-Visualizer`

| Prime Spiral (Green-Blue) | Multiples of 111 (Red-Black) |
| :-----------------------: | :--------------------------: |
| ![Primes](screenshots/preview.gif) | ![111s](screenshots/multiples_111.gif) |
