# Robot War Simulator

This project is a C++ application that simulates an autonomous robot battlefield on a 2D grid. The simulation is turn-based, with each robot performing a sequence of actions before the next turn begins.

---

## Key Features

Customizable Battlefield: The battlefield dimensions and the number of turns can be configured by modifying the setup.txt file.

Diverse Robot Types: Robots are initialized as one of four types: Generic Robot, Tank Robot, Sniper Robot, or Fast Robot, each with unique attributes and behaviors.

Generic Robot: A standard robot with a base set of abilities.

Tank Robot: More durable than other robots, starting with 4 lives.

Sniper Robot: Can shoot enemies from a longer distance (up to 2 units).

Fast Robot: Has enhanced mobility and can move up to two steps per turn.

Autonomous Decision-Making: Each robot follows a fixed action sequence: LOOK → THINK → SHOOT → MOVE.

Dynamic Upgrade System: Robots can earn upgrades by eliminating enemies. Available upgrades include enhancements for Movement (HideBot or JumpBot), Shooting (LongShotBot, SemiAutoBot, or ThirtyShotBot), Seeing (ScoutBot or TrackBot), and Healing. The robot randomly selects an upgrade from the available categories.

Automated Results: All actions and events during the simulation are logged with a timestamp in a text file, providing a detailed record of the battle.

Final Winner Determination: The simulation ends when either all robots are destroyed, or only one robot remains. It also concludes if the maximum number of turns is reached.

---

## How to Run the Simulation

Compile the Program: Use a C++ compiler (e.g., g++) to compile the source code. You can run the following command in your terminal or command prompt:
~~~
g++ Assignment.cpp -o a.exe
~~~

Configure the Battlefield: Open the setup.txt file to set the battlefield dimensions, the number of simulation steps, and the initial positions and types of the robots.

Run the Simulation: Execute the compiled program from the command line:
~~~
./a.exe
~~~

Observe the Output: The simulation will display the battlefield grid and robot actions in the console. A log file with the date and time in its name will be automatically generated, recording all actions and the final winner.

---

## Logic for Robot Actions
The robots' actions are guided by a simple, turn-based logic. During its turn, a robot first uses its "seeing" ability to look for enemies. Then, based on its "thinking" process (which is currently implemented as a random upgrade choice ), it decides whether to "shoot" or "move".

The "think" function is a key part of the robot's logic. When a robot successfully hits an enemy, it is marked for an upgrade. In its next turn, it checks for available upgrades and randomly chooses one to apply. For example, if a robot chooses a "shooting" upgrade, it might randomly become a 

LongShotBot, SemiAutoBot, or a ThirtyShotBot. These upgrades permanently alter the robot's capabilities, influencing its future actions and giving it a strategic advantage in the battle. The decision-making is currently random but demonstrates the foundation for a more complex AI.

---

## Group Member
1. LIEW WEN XING
2. ONG JING RU
3. KUMAR,VIPUL
