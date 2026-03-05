# Robot Assembly Interaction System (Unreal Engine)

This project demonstrates a modular interaction system built in Unreal Engine using C++.
Players can pick up robot parts, drag them around the environment, and attach them to compatible sockets.

The project focuses on clean gameplay architecture using components, interfaces, and the strategy pattern.

---

## Key Systems Demonstrated

- Component-driven gameplay architecture
- Interface-based interaction system (IInteractable)
- Strategy pattern for highlighting
- Modular robot attachment system
- Unreal Automation Tests

---

## Setup

1. Right-click `RobotAbuse.uproject` and select **Generate Visual Studio project files**
2. Open `RobotAbuse.sln` in **Visual Studio** or **JetBrains Rider**
3. Build the solution in Visual Studio or Rider.
4. Launch `RobotAbuse.uproject`
5. Open level: Content → Levels → MainLevel

---

## Controls

### Camera

WASD — Move camera  
Hold Right Mouse Button — Rotate camera  

### Interaction

Left Click on arm — Pick up individual arm  
Left Click on torso — Pick up entire robot  
Left Click on attachment sphere — Attach arm to socket (if compatible)  
Left Click elsewhere — Drop held object  



