# Pacman Game - LPC1768 LandTiger Board
**EXTRAPOINT 2 - Architetture dei sistemi di elaborazione (ASE) Exam Project**

## 🎥 Video Demonstration
> **📹 [WATCH THE COMPLETE PROJECT DEMONSTRATION](./extraPoint_1_Videox1.25_Spiegazione_Necerini.mp4)**  
> *Complete walkthrough showing all game functionalities, hardware interactions, and system behavior in action*

---

## 📋 Project Overview

This project implements a fully functional **Pacman game** on the **LPC1768 LandTiger development board**, demonstrating advanced embedded systems programming techniques including real-time system design, hardware interfacing, and AI algorithms.

## 🎯 Key Features

### Game Mechanics
- **Complete Pacman gameplay** with authentic maze layout (28x31 grid)
- **Intelligent ghost AI** using A* pathfinding algorithm
- **Multiple game modes**: Chase, Frightened, and Game Over
- **Score tracking** and **lives management**
- **Power pills** that change ghost behavior
- **Collision detection** and game state management

### Hardware Integration
- **LCD Display** (240x320 pixels) with custom graphics rendering
- **5-direction Joystick** control with hardware debouncing
- **External Interrupt Buttons** (KEY1, KEY2, KEY3) for game control
- **LED Array** for visual feedback and status indication
- **CAN Bus Communication** for inter-board messaging
- **DAC Audio Synthesis** for dynamic background music

### Technical Implementation
- **Real-Time System Architecture** with interrupt-driven design
- **Multi-Timer Management**:
  - Timer0/1: Game logic and timing (125ms, 1s periods)
  - Timer2/3: Audio synthesis and note generation
  - RIT: 50ms periodic updates for smooth gameplay
- **ARM Assembly Integration** for performance-critical operations
- **Advanced Pathfinding**: A* algorithm for intelligent ghost movement
- **Memory-Optimized Graphics** for embedded display rendering

## 🛠 Technical Specifications

### Hardware Platform
- **Microcontroller**: ARM Cortex-M3 LPC1768 (100MHz)
- **Development Board**: Keil LandTiger LPC1768
- **Display**: 240x320 pixel LCD with touch panel support
- **Memory**: 512KB Flash, 64KB SRAM

### Software Architecture
- **Programming Languages**: C and ARM Assembly
- **Development Environment**: Keil µVision IDE
- **Real-Time Design**: Interrupt Service Routines (ISR) based architecture
- **Modular Structure**: Organized in functional modules (Pacman, Ghost, Labyrinth, etc.)

## 📁 Project Structure

```
Source/
├── sample.c                 # Main application entry point
├── asm_function.s          # ARM Assembly optimized functions
├── Pacman/                 # Pacman character implementation
├── Ghost/                  # Ghost AI and behavior
├── Astar/                  # A* pathfinding algorithm
├── labyrinth/              # Maze generation and rendering
├── GLCD/                   # Graphics LCD driver
├── joystick/               # Joystick input handling
├── button_EXINT/           # External interrupt buttons
├── timer/                  # Timer configuration and ISRs
├── RIT/                    # Repetitive Interrupt Timer
├── CAN/                    # CAN bus communication
├── led/                    # LED control functions
├── music/                  # Audio synthesis system
└── TouchPanel/             # Touch panel interface
```

## 🎮 Game Controls

- **Joystick**: Move Pacman (Up/Down/Left/Right/Select)
- **KEY1 (INT0)**: Pause/Resume game
- **KEY2**: Game mode control
- **KEY3**: Reset/Special functions

## 🎵 Audio System

The project features a sophisticated audio system with:
- **Dynamic music generation** using DAC output
- **Multiple melodies** for different game states:
  - Chase mode: Fast-paced pursuit theme
  - Frightened mode: Tension-building sequence
  - Game Over: Conclusion melody
- **Real-time audio synthesis** with timer-controlled note generation

## 🧠 AI Implementation

### A* Pathfinding Algorithm
- **Intelligent ghost movement** toward Pacman's position
- **Obstacle avoidance** in maze environment
- **Heuristic-based optimization** for efficient pathfinding
- **Dual behavior modes**: Chase and Frightened with different strategies

## ⚙️ Compilation and Setup

### Prerequisites
- Keil µVision IDE (version 5 or later)
- ARM compiler for Cortex-M3
- LPC1768 LandTiger development board

### Build Instructions
1. Open `sample.uvprojx` in Keil µVision
2. Select appropriate target configuration
3. Build the project (F7)
4. Flash to LPC1768 board
5. Connect hardware peripherals as per schematic

## 🔧 Hardware Configuration

### Pin Assignments
- **LCD Interface**: Parallel connection via GPIO
- **Joystick**: P1.25-P1.29 (5-direction input)
- **External Interrupts**: P2.10-P2.12 (KEY1-KEY3)
- **CAN Bus**: P0.0-P0.1 (CAN1), P0.4-P0.5 (CAN2)
- **LEDs**: P2.0-P2.7 (8-LED array)
- **DAC Output**: P0.26 (Audio)

## 📊 Performance Characteristics

- **Game Loop**: 50ms update cycle via RIT
- **Display Refresh**: Optimized partial updates
- **AI Processing**: Sub-millisecond pathfinding execution
- **Audio Latency**: Real-time synthesis with <1ms delay
- **Memory Usage**: Efficient embedded graphics and game state management

## 🎓 Educational Objectives

This project demonstrates proficiency in:
- **Embedded Systems Design**
- **Real-Time Programming**
- **Hardware-Software Integration**
- **Algorithm Implementation** (A* pathfinding)
- **Interrupt-Driven Architecture**
- **Digital Signal Processing** (Audio)
- **Human-Machine Interface Design**

## 👥 Development

**Author**: Ivan Nece  
**Course**: Architetture dei sistemi di elaborazione (ASE)  
**Institution**: Politecnico di Torino  
**Academic Year**: 2024-2025

---

*This project represents a comprehensive embedded systems implementation, showcasing advanced programming techniques and real-time system design principles on ARM Cortex-M3 architecture.*
