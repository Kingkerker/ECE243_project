# Tank Battle Arena - Two-Player FPGA Combat Game

A real-time two-player tank battle game implemented on FPGA hardware, featuring projectile physics, dynamic obstacles, shield power-ups, and 7-segment display scoring.

## 🎮 Game Overview

Two tanks face off in an arena filled with obstacles that affect movement and bullet trajectories. The first player to land a hit on their opponent wins the round. A shield power-up provides temporary protection, and the game tracks victories across multiple rounds.

## 🎯 Features

- **Two-Player Local Multiplayer**: Head-to-head combat on a single system
- **Projectile System**: Each tank can fire up to 3 active bullets simultaneously
- **Realistic Bullet Physics**: Bullets bounce off walls and obstacles
- **Dynamic Obstacles**: Randomly generated barriers that block movement and deflect bullets
- **Shield Power-up**: Central shield pickup grants protection against one hit
- **Score Tracking**: Victory points displayed on 7-segment displays (HEX0-5)
- **Instant Victory Detection**: Immediate game end with sound effect and victory message upon hit
- **Reset Functionality**: Quick restart with space key

## 🕹️ Controls

| Action | Player 1 (Left Tank) | Player 2 (Right Tank) |
|--------|---------------------|----------------------|
| Move Up | W | ↑ (Up Arrow) |
| Move Down | S | ↓ (Down Arrow) |
| Move Left | A | ← (Left Arrow) |
| Move Right | D | → (Right Arrow) |
| Fire | Q | M |

**System Controls:**
- **Space**: Reset game after a round ends

## 🎲 Game Mechanics

### Movement
- Tanks navigate through the arena with obstacles blocking movement
- Each tank starts on opposite sides of the map
- Movement respects collision detection with walls and obstacles

### Combat System
- Maximum of 3 bullets per tank active at any time
- Bullets travel in straight lines until hitting:
  - Walls: Bullet bounces
  - Obstacles: Bullet bounces off
  - Tank: Game ends with victory for the shooter
- Sound effect plays when a tank is hit

### Shield System
- A shield power-up spawns at the center of the map
- Tanks can collect the shield by passing through it
- Shield protects against the next bullet hit
- Shield is consumed after blocking one attack

### Scoring & Victory
- Victory message displays "T1 W" (Tank 1 Wins) or "T2 W" (Tank 2 Wins)
- Winning tank receives 1 point
- Scores displayed on 7-segment displays:
  - HEX0-1: Player 1 score
  - HEX2-3: Player 2 score
  - HEX4-5: Reserved or round indicator
- Game ends immediately when a tank is hit
- Press space to reset and start a new round

## 🛠️ Hardware Requirements

### FPGA Board
- Compatible with standard FPGA development boards (DE10-Lite, DE2-115, etc.)
- 7-segment display outputs (HEX0-HEX5)
- VGA output for game display
- Audio output for sound effects
- PS/2 or USB keyboard input support

### Required Components
- **Input**: Keyboard controller interface for two-player simultaneous input
- **Output**: VGA controller for 640x480 resolution (or similar)
- **Display**: 6-digit 7-segment display module
- **Audio**: Simple buzzer or PWM audio output

## 📁 Project Structure
