This is a **puzzle game of the "Eight Queens"**
written in C++ with Qt6

A multiplatform opensource QUEENs game! 
**Under very recent development..**

# QtQueens Puzzle App

A graphical and interactive implementation of the classic **N-Queens Puzzle** using the Qt framework.

---

## Features

### 🎨 **Dynamic Chessboard**
- Supports dynamic resizing of the chessboard (e.g., 8x8, 4x4, etc. up to 25x25 ).
- Automatically adjusts square sizes for any board dimension.

### 👑 **Interactive Gameplay**
- **Conflicts Check**: Queens in conflict are visually highlighted in red.
- **Hint System**: Helps you resolve conflicts by suggesting safe moves.

### ⏱️ **Chronometer**
- Tracks the elapsed time from the start of the game.
- Automatically resets when the board size is changed.

### 🛠️ **Automated Solver**
- Solves the N-Queens puzzle for the current board size using the **backtracking algorithm**.
- Animates the solving process for better visualization.

---

## How It Works

1. **Place Queens Manually**:
   - Drag queens on the board and manually find a non-conflicting arrangement.

2. **Hint Button**:
   - If you are stuck, click the "Hint" button to identify conflicts and receive safe move suggestions. If there are any! MWawhawhawhw

3. **Solve Automatically**:
   - Use the solver to let the app automatically resolve the puzzle for you.

---

## Visual Feedback

- **Red Queens**: Queens in conflict are tinted red.
- **Green Highlights**: Safe positions suggested by the "Hint" system are highlighted in green.

---

## Controls

- **Mouse Drag**: Drag queens on the board!
- **Hint Button**: Suggests safe moves for queens causing conflicts.
- **Automatic Solver**: Resolves the N-Queens puzzle for you. Maximum 25x25 board size.

---

## Technologies Used

- **C++**: Core logic implementation.
- **Qt Framework**: Graphics and GUI functionalities.

---

## Installation

1. Clone this repository and build with cmake:
```bash
   git clone https://github.com/guprobr/QtQueens
   cd Qtqueens
   mkdir build
   cd build
   cmake ..
   make
```
![image](https://github.com/user-attachments/assets/2b702e8f-0a95-4359-aab7-5c6193c7339c)

