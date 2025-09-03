/**********|**********|**********|
Program: main.cpp
Course: Object-Oriented Programming and Data Structure
Name: LIEW WEN XING | ONG JING RU | KUMAR,VIPUL
Lecture Section: TC4L
Tutorial Section: T14L
**********|**********|**********/

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <ctime>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <random>
using namespace std;

ofstream logFile;
void logAction(const string &message)
{
    if (logFile.is_open())
    {
        logFile << message << endl;
    }
}

enum HitLogicType {
    DEFAULT_HIT_LOGIC,
    SEMI_AUTO_LOGIC,
    SNIPER_LOGIC,
    LONG_SHOT_LOGIC,
    CUSTOM_HIT_LOGIC  // for future flexibility
};

class ThinkingRobot;
class SeeingRobot;
class ShootingRobot;
class MovingRobot;
class HealingRobot;

class Battlefield;
class HideBotAbility;
class JumpBotAbility;
class ScoutBotAbility;
class TrackBotAbility;
class HealBotAbility;

class Robot
{
protected:
    enum Action
    {
        THINK,
        MOVE,
        SEE,
        SHOOT,
        ACTION_COUNT
    };

    enum UpgradeArea
    {
        MOVING,
        SHOOTING,
        SEEING,
        HEALING,
        UPGRADE_AREA_COUNT
    };

    enum SeeingUpgrade
    {
        NONE,
        SCOUT_BOT,
        TRACK_BOT
    };

    enum MovingUpgrade
    {
        NO_MOVING_UPGRADE,
        HIDE_BOT,
        JUMP_BOT
    };

    enum ShootingUpgrade
    {
        S_NONE,
        S_LONG_SHOT,
        S_SEMI_AUTO,
        S_THIRTY_SHOT
    };

    // Upgrade state variables (moved from derived classes)
    SeeingUpgrade seeingUpgrade;
    MovingUpgrade movingUpgrade;
    ShootingUpgrade shootingUpgrade;
    bool hasHealBotUpgrade;

    // Upgrade ability pointers
    unique_ptr<ScoutBotAbility> scoutAbility;
    unique_ptr<TrackBotAbility> trackAbility;
    unique_ptr<HideBotAbility> hideAbility;
    unique_ptr<JumpBotAbility> jumpAbility;
    unique_ptr<HealBotAbility> healAbility;

    // Helper functions
    bool shouldUseScoutVision() const { return rand() % 2 == 0; }
    bool shouldUseTracker() const { return rand() % 2 == 0; }
    bool shouldUseHide() const { return rand() % 2 == 0; }
    bool shouldUseJump() const { return rand() % 2 == 0; }
    bool shouldUseLongShot() const { return rand() % 2 == 0; }
    bool shouldUseSemiAuto() const { return rand() % 2 == 0; }

    string name;
    int row;
    int col;
    char symbol;
    int lives;
    int shells;

    static int battlefieldRows;
    static int battlefieldCols;
    static Battlefield *battlefield;

    void basicLook();
    void basicThink();
    bool basicShoot();
    void basicMove();

    pair<int, int> findValidMovePosition();

    bool enemyDetected;
    vector<pair<int, int>> detectedEnemyPositions;

    vector<UpgradeArea> chosenUpgrades;

public:
    Robot(const string &robotName, int r, int c);
    virtual ~Robot();

    // Operator overloading for output
    friend ostream &operator<<(ostream &os, const Robot &robot)
    {
        os << robot.name << " (Lives:" << robot.lives
           << ", Shells:" << robot.shells << ") @ ("
           << robot.row << "," << robot.col << ")";
        return os;
    }
    virtual void performTurnActions();

    string getName() const {return name;}
    int getRow() const {return row;}
    int getCol() const {return col;}
    char getSymbol() const {return symbol;}
    int getLives() const {return lives;}
    void loseLives();
    bool isAlive() const;

    void destroy();

    int getShells() const {return shells;}
    void useShell();

    virtual Action chooseAction();

    static void setBattlefieldSize(int r, int c);
    static void setBattlefield(Battlefield *bf);

    virtual void see();
    virtual void think();
    virtual bool shoot();
    virtual void move();

    bool canUpgrade(UpgradeArea area) const;
    void applyUpgrade(UpgradeArea area);
    static string getAreaName(UpgradeArea area);
    bool shouldUpgrade = false;
    void triggerUpgradeChoice();
    void markForUpgrade() { shouldUpgrade = true; }
    void setPosition(int r, int c);

    // Upgrade interface functions (implement in Robot)
    void upgradeTo(SeeingUpgrade upgrade);
    void chooseSeeingUpgrade();
    void upgradeToMoving(MovingUpgrade upgrade);
    void chooseMovingUpgrade();
    void chooseShootingUpgrade();
    bool handleSemiAuto(Robot* attacker, int dx, int dy);
    void chooseHealingUpgrade();
    // Add to existing Robot class
    virtual void applySpecificUpgrades() = 0; // For derived class customization
};

class Battlefield {
private:
    int rows, cols, steps;
    vector<vector<char>> grid;
    vector<unique_ptr<Robot>> robots;

public:
    int getRows() const { return rows; }
    int getCols() const { return cols; }
    Battlefield();
    bool readFile(const string &filename);
    void updateGrid();
    void display() const;
    void simulate();
    void spawnRobot(const string &type, const string &name, const string &rowStr, const string &colStr);
    Robot *findTargetAt(int row, int col, Robot *attacker);
    bool positionOccupied(int r, int c);
    bool handleCombatUnified(Robot* attacker, int targetX, int targetY,
                                          const string& combatType, const string& description,
                                          HitLogicType logicType);
    bool handleCombat(Robot *attacker, int targetRow, int targetCol);
    bool handleSniperCombat(Robot *attacker, int dx, int dy);
    bool handleLongCombat(Robot *attacker, int dx, int dy);
    bool handleSemiAutoCombat(Robot* attacker, int dx, int dy);
    vector<pair<int, int>> lookAround(Robot *robot, int dx, int dy);

    // Help method for scout
    vector<Robot *> getAllAliveRobots() const
    {
        vector<Robot *> aliveRobots;
        for (const auto &robot : robots)
        {
            if (robot->isAlive())
            {
                aliveRobots.push_back(robot.get());
            }
        }
        return aliveRobots;
    }

    // Help method to find nearby robots
    Robot *findNearestRobot(Robot *searcher) const
    {
        Robot *nearest = nullptr;
        int minDistance = INT_MAX;

        for (const auto &robot : robots)
        {
            if (robot.get() != searcher && robot->isAlive())
            {
                int distance = abs(robot->getRow() - searcher->getRow()) +
                               abs(robot->getCol() - searcher->getCol());
                if (distance < minDistance)
                {
                    minDistance = distance;
                    nearest = robot.get();
                }
            }
        }
        return nearest;
    }
};

class HideBotAbility
{
private:
    int hideUsesLeft = 3;
    bool currentlyHidden = false;
    int hideDuration = 0; // How many turns the robot stays hidden

public:
    bool hasUsesLeft() const { return hideUsesLeft > 0; }
    bool isHidden() const { return currentlyHidden; }

    bool activateHide(Robot *robot)
    {
        if (hideUsesLeft > 0 && !currentlyHidden)
        {
            hideUsesLeft--;
            currentlyHidden = true;
            hideDuration = 3; // Hide for 3 turns
            cout << robot->getName() << " activates hide mode! (Invisible for 3 turns, "
                 << hideUsesLeft << " hide uses remaining)\n";
            logAction("HIDE_ACTIVATED: " + robot->getName() + " activated hide mode (" +
                      to_string(hideUsesLeft) + " uses remaining)");
            return true;
        }
        return false;
    }

    void updateHideStatus(Robot *robot)
    {
        if (currentlyHidden)
        {
            hideDuration--;
            if (hideDuration <= 0)
            {
                currentlyHidden = false;
                cout << robot->getName() << " is no longer hidden.\n";
                logAction("HIDE_ENDED: " + robot->getName() + " is no longer hidden");
            }
        }
    }
};

// JumpBot functionality
class JumpBotAbility
{
private:
    int jumpUsesLeft = 3;

public:
    bool hasUsesLeft() const { return jumpUsesLeft > 0; }

    bool performJump(Robot *robot, int targetRow, int targetCol, int maxRows, int maxCols)
    {
        if (jumpUsesLeft > 0)
        {
            // Validate target position
            if (targetRow >= 0 && targetRow < maxRows && targetCol >= 0 && targetCol < maxCols)
            {
                jumpUsesLeft--;
                cout << robot->getName() << " jumps from (" << robot->getRow() << "," << robot->getCol()
                     << ") to (" << targetRow << "," << targetCol << ")! ("
                     << jumpUsesLeft << " jumps remaining)\n";
                logAction("JUMP: " + robot->getName() + " jumped from (" +
                          to_string(robot->getRow()) + "," + to_string(robot->getCol()) +
                          ") to (" + to_string(targetRow) + "," + to_string(targetCol) +
                          ") - " + to_string(jumpUsesLeft) + " jumps remaining");
                return true;
            }
        }
        return false;
    }
};

// Enhanced ScoutBot functionality
class ScoutBotAbility
{
private:
    int scoutVisionUses = 3;
    vector<pair<int, int>> visiblePositions;

public:
    bool hasUsesLeft() const { return scoutVisionUses > 0; }

    void performScoutVision(Robot *robot, Battlefield *battlefield)
    {
        if (scoutVisionUses > 0)
        {
            visiblePositions.clear();
            vector<Robot *> allRobots = battlefield->getAllAliveRobots();

            for (Robot *otherRobot : allRobots)
            {
                if (otherRobot != robot)
                {
                    visiblePositions.push_back({otherRobot->getRow(), otherRobot->getCol()});
                }
            }

            scoutVisionUses--;
            displayVisibleRobots(robot->getName());
            logAction("SCOUT_VISION: " + robot->getName() + " used scout vision (" +
                      to_string(scoutVisionUses) + " uses remaining)");
        }
        else
        {
            cout << robot->getName() << " has no scout vision uses left." << endl;
        }
    }

    void displayVisibleRobots(const string &robotName) const
    {
        cout << robotName << " (Scout Vision) sees the following robots:\n";
        for (const auto &[r, c] : visiblePositions)
        {
            cout << " - Robot at (" << r << ", " << c << ")\n";
        }
    }
    bool foundEnemies() const
    {
        return !visiblePositions.empty();
    }
};

// Enhanced TrackBot functionality
class TrackBotAbility
{
private:
    int trackersLeft = 3;
    vector<Robot *> trackedRobots;

public:
    bool hasTrackersLeft() const { return trackersLeft > 0; }

    const vector<Robot *> &getTrackedRobots() const { return trackedRobots; }

    void cleanUpDeadTrackedRobots()
    {
        auto it = trackedRobots.begin();
        while (it != trackedRobots.end())
        {
            if (!(*it)->isAlive())
            {
                it = trackedRobots.erase(it);
                trackersLeft++;
            }
            else
            {
                ++it;
            }
        }
    }

    bool plantTracker(Robot *tracker, Robot *target)
    {
        cleanUpDeadTrackedRobots();

        if (trackedRobots.size() >= 3)
        {
            cout << tracker->getName() << " cannot track more than 3 robots.\n";
            return false;
        }

        if (trackersLeft > 0 && target && target != tracker)
        {
            for (Robot *r : trackedRobots)
            {
                if (r == target)
                {
                    cout << tracker->getName() << " is already tracking " << target->getName() << ".\n";
                    return false;
                }
            }

            trackedRobots.push_back(target);
            trackersLeft--;
            cout << tracker->getName() << " planted a tracker on " << target->getName() << ".\n";
            logAction("TRACKER_PLANTED: " + tracker->getName() + " planted tracker on " + target->getName());
            return true;
        }

        return false;
    }

    void displayTrackedLocations(const string &robotName) const
    {
        if (trackedRobots.empty())
        {
            cout << " [none]" << endl;
            return;
        }

        bool any = false;
        for (Robot *robot : trackedRobots)
        {
            if (robot->isAlive())
            {
                cout << " - " << robot->getName() << " at (" << robot->getRow() << ", " << robot->getCol() << ")\n";
                any = true;
            }
            else
            {
                cout << " - " << robot->getName() << " [DESTROYED]\n";
                any = true;
            }
        }

        if (!any)
            cout << " [none]" << endl;
    }

    bool hasActiveTrack() const
    {
        for (Robot *r : trackedRobots)
        {
            if (r->isAlive())
                return true;
        }
        return false;
    }
};

class HealBotAbility
{
public:
    virtual ~HealBotAbility() = default;
};

class GenericRobot : public Robot
{
public:
    using Robot::Robot;                      // Inherit constructor
    void applySpecificUpgrades() override {} // Add any specific logic
};

class SniperRobot : public Robot
{
public:
    using Robot::Robot;                      // Inherit constructor
    void applySpecificUpgrades() override {} // Add any specific logic

    bool shoot() override;
};

class TankRobot : public Robot
{
public:
    TankRobot(const string& name, int row, int col) : Robot(name, row, col)
    {
        lives = 4;
    }
    void applySpecificUpgrades() override {}
};

class FastRobot : public Robot
{
public:
    using Robot::Robot;

    void applySpecificUpgrades() override {}

    void see() override { Robot::see(); }
    void think() override { Robot::think(); }
    bool shoot() override { return Robot::shoot(); }
    void move() override;
};

Robot::Robot(const string &robotName, int r, int c)
    : name(robotName), row(r), col(c), symbol(name[0]), lives(3), shells(10),
      seeingUpgrade(NONE), movingUpgrade(NO_MOVING_UPGRADE),
      shootingUpgrade(S_NONE), hasHealBotUpgrade(false)
{}

Robot::~Robot() = default;

int Robot::battlefieldRows = 0;
int Robot::battlefieldCols = 0;
Battlefield *Robot::battlefield = nullptr;

void Robot::setBattlefieldSize(int r, int c) { battlefieldRows = r; battlefieldCols = c; }
void Robot::setBattlefield(Battlefield *bf) { battlefield = bf; }

void Robot::think()
{
    basicThink();
}

void Robot::move()
{
    // Try JumpBot ability
    if (movingUpgrade == JUMP_BOT && jumpAbility && jumpAbility->hasUsesLeft())
    {
        int randRow = rand() % battlefieldRows;
        int randCol = rand() % battlefieldCols;

        if (jumpAbility->performJump(this, randRow, randCol, battlefieldRows, battlefieldCols))
            return; // Jump successful
    }

    // Try HideBot ability
    if (movingUpgrade == HIDE_BOT && hideAbility && hideAbility->hasUsesLeft())
    {
        if (shouldUseHide()) // Random trigger
        {
            if (hideAbility->activateHide(this))
                return; // Hide successful
        }
    }

    // Fallback to normal movement
    basicMove();
}

void Robot::see()
{
    // Try ScoutBot vision first
    if (seeingUpgrade == SCOUT_BOT && scoutAbility && scoutAbility->hasUsesLeft())
    {
        if (shouldUseScoutVision())
        {
            scoutAbility->performScoutVision(this, battlefield);
            return; // Skip normal look
        }
    }

    // Try to plant tracker if TrackBot
    if (seeingUpgrade == TRACK_BOT && trackAbility && trackAbility->hasTrackersLeft())
    {
        if (shouldUseTracker() && !detectedEnemyPositions.empty())
        {
            auto [r, c] = detectedEnemyPositions[rand() % detectedEnemyPositions.size()];
            Robot* target = battlefield->findTargetAt(r, c, this);
            if (target)
                trackAbility->plantTracker(this, target);
        }
    }
    basicLook();
}


bool Robot::shoot() {
    if (!battlefield || shells == 0) return false;

    switch (shootingUpgrade) {
        case S_SEMI_AUTO:
            if (shouldUseSemiAuto() && !detectedEnemyPositions.empty()) {
                auto [targetRow, targetCol] = detectedEnemyPositions[rand() % detectedEnemyPositions.size()];
                int dx = targetRow - row;
                int dy = targetCol - col;
                if (abs(dx) <= 1 && abs(dy) <= 1) {
                    return battlefield->handleSemiAutoCombat(this, dx, dy);
                }
            }
            break;

        case S_LONG_SHOT:
            if (shouldUseLongShot() && !detectedEnemyPositions.empty()) {
                auto [targetRow, targetCol] = detectedEnemyPositions[rand() % detectedEnemyPositions.size()];
                int dx = targetRow - row;
                int dy = targetCol - col;
                return battlefield->handleLongCombat(this, dx, dy);
            }
            break;

        case S_THIRTY_SHOT:
        case S_NONE:
        default:
            return basicShoot();
    }

    // Fallback to basic shooting
    return basicShoot();
}

void Robot::basicThink()
{
    cout << name << " is thinking strategically" << endl;
    logAction("THINK: " + name + " is analyzing situation");
}

void Robot::basicLook() {
    detectedEnemyPositions = battlefield->lookAround(this, 0, 0);
    enemyDetected = !detectedEnemyPositions.empty();
}

bool Robot::basicShoot() {
    if (!battlefield || shells == 0)
        return false;

    if (!enemyDetected || detectedEnemyPositions.empty()) {
        cout << "No enemies nearby, not shooting." << endl;
        return false;
    }

    auto [targetRow, targetCol] = detectedEnemyPositions[rand() % detectedEnemyPositions.size()];

    if (abs(targetRow - row) > 1 || abs(targetCol - col) > 1) {
        cout << "Enemy too far, not shooting." << endl;
        return false;
    }

    return battlefield->handleCombat(this, targetRow, targetCol);
}

void Robot::basicMove()
{
    auto [newRow, newCol] = findValidMovePosition();

    if (newRow != row || newCol != col)
    {
        int oldRow = row, oldCol = col;
        row = newRow;
        col = newCol;
        cout << name << " moves to (" << row << "," << col << ")" << endl;
        logAction("MOVE: " + name + " moved from (" + to_string(oldRow) + "," + to_string(oldCol) +
                  ") to (" + to_string(row) + "," + to_string(col) + ")");
    }
    else
    {
        cout << name << " stays at (" << row << "," << col << ")" << endl;
    }
}

pair<int, int> Robot::findValidMovePosition()
{
    for (int tries = 0; tries < 10; tries++)
    {
        int direction = rand() % 8;
        int newRow = row, newCol = col;

        switch (direction)
        {
        case 0:
            newRow = max(0, row - 1);
            break;
        case 1:
            newRow = min(battlefieldRows - 1, row + 1);
            break;
        case 2:
            newCol = max(0, col - 1);
            break;
        case 3:
            newCol = min(battlefieldCols - 1, col + 1);
            break;
        case 4:
            newRow = max(0, row - 1);
            newCol = max(0, col - 1);
            break;
        case 5:
            newRow = max(0, row - 1);
            newCol = min(battlefieldCols - 1, col + 1);
            break;
        case 6:
            newRow = min(battlefieldRows - 1, row + 1);
            newCol = max(0, col - 1);
            break;
        case 7:
            newRow = min(battlefieldRows - 1, row + 1);
            newCol = min(battlefieldCols - 1, col + 1);
            break;
        }

        if (!battlefield || !battlefield->positionOccupied(newRow, newCol))
        {
            return {newRow, newCol};
        }
    }
    return {row, col};
}

void Robot::loseLives()
{
    if (lives > 0)
    {
        lives--;
        logAction("DAMAGE: " + name + " lost a life (Lives remaining: " + to_string(lives) + ")");
        if (lives == 0)
        {
            destroy();
        }
    }
}

bool Robot::isAlive() const
{
    return lives > 0 && shells > 0;
}

void Robot::destroy()
{
    lives = 0;
    shells = 0;
    logAction("DESTROYED: " + name + " has been destroyed!");
}

void Robot::useShell()
{
    if (shells > 0)
    {
        shells--;
        if (shells == 0)
        {
            logAction("OUT_OF_AMMO: " + name + " has no shells left!");
            destroy();
        }
    }
}

Robot::Action Robot::chooseAction()
{
    return static_cast<Action>(rand() % 4);
}

void Robot::performTurnActions()
{
    if (movingUpgrade == HIDE_BOT && hideAbility)
    hideAbility->updateHideStatus(this);

    cout << "[LOOK] ";
    see();

    cout << "[THINK] ";
    think();

    cout << "[SHOOT] ";
    shoot();

    cout << "[MOVE] ";
    move();

    if (shouldUpgrade)
    {
        // Check if any upgrades are actually available before showing the frame
        vector<UpgradeArea> availableAreas;
        if (canUpgrade(MOVING))
            availableAreas.push_back(MOVING);
        if (canUpgrade(SHOOTING))
            availableAreas.push_back(SHOOTING);
        if (canUpgrade(SEEING))
            availableAreas.push_back(SEEING);
        if (canUpgrade(HEALING))
            availableAreas.push_back(HEALING);

        if (!availableAreas.empty())
        {
            cout << "\n*** UPGRADE OPPORTUNITY ***" << endl;
            triggerUpgradeChoice();
            cout << "*** END UPGRADE ***" << endl;
        }
        else
        {
            triggerUpgradeChoice();
        }

        shouldUpgrade = false;
    }
}

bool Robot::canUpgrade(UpgradeArea area) const
{
    if (chosenUpgrades.size() >= 3)
        return false;
    for (UpgradeArea chosen : chosenUpgrades)
    {
        if (chosen == area)
            return false;
    }
    return true;
}

void Robot::applyUpgrade(UpgradeArea area) {
    if (!canUpgrade(area)) {
        cout << name << " cannot upgrade " << getAreaName(area) << " again." << endl;
        return;
    }

    chosenUpgrades.push_back(area);
    logAction("UPGRADE: " + name + " upgraded " + getAreaName(area));

    switch (area) {
        case MOVING:
            chooseMovingUpgrade();
            break;
        case SEEING:
            chooseSeeingUpgrade();
            break;
        case SHOOTING:
            chooseShootingUpgrade();
            break;
        case HEALING:
            chooseHealingUpgrade();
            break;
    }
    applySpecificUpgrades();
}

string Robot::getAreaName(UpgradeArea area) {
    switch (area) {
        case MOVING: return "Moving";
        case SHOOTING: return "Shooting";
        case SEEING: return "Seeing";
        case HEALING: return "Healing";
        default: return "Unknown";
    }
}

void Robot::triggerUpgradeChoice()

{
    // First check if any upgrades are available
    vector<UpgradeArea> availableAreas;

    if (canUpgrade(MOVING))
        availableAreas.push_back(MOVING);
    if (canUpgrade(SHOOTING))
        availableAreas.push_back(SHOOTING);
    if (canUpgrade(SEEING))
        availableAreas.push_back(SEEING);
    if (canUpgrade(HEALING))
        availableAreas.push_back(HEALING);

    // Only show upgrade opportunity if there are available upgrades
    if (!availableAreas.empty())
    {
        cout << "\n"
             << name << " hit an enemy and can choose an upgrade!" << endl;
        cout << "Available upgrade areas:" << endl;

        for (UpgradeArea area : availableAreas)
        {
            cout << " - " << getAreaName(area) << endl;
        }

        // Randomly choose an upgrade
        int choice = rand() % availableAreas.size();
        UpgradeArea chosenArea = availableAreas[choice];
        cout << name << " chooses to upgrade " << getAreaName(chosenArea) << "!" << endl;
        applyUpgrade(chosenArea);
    }
    else
    {
        cout << "\n"
             << name << " hit an enemy but has already reached maximum upgrades (3/3)." << endl;
        cout << "No upgrade available." << endl;
    }
}

void Robot::setPosition(int r, int c)
{
    row = r;
    col = c;
}


void Robot::chooseMovingUpgrade()
{
    cout << "\nMoving upgrade for " << name << ":" << endl;
    cout << "1. HideBot - Can hide from attacks (3 uses, 3 turns each)" << endl;
    cout << "2. JumpBot - Can jump to any location on the map (3 uses)" << endl;

    // For simulation, randomly choose
    int choice = (rand() % 2) + 1;

    switch (choice)
    {
    case 1:
        upgradeToMoving(HIDE_BOT);
        cout << name << " upgraded to HideBot!" << endl;
        logAction("MOVING_UPGRADE: " + name + " upgraded to HideBot");
        break;
    case 2:
        upgradeToMoving(JUMP_BOT);
        cout << name << " upgraded to JumpBot!" << endl;
        logAction("MOVING_UPGRADE: " + name + " upgraded to JumpBot");
        break;
    }
}

void Robot::upgradeToMoving(MovingUpgrade upgrade)
{
    movingUpgrade = upgrade;

    switch (upgrade)
    {
    case HIDE_BOT:
        hideAbility = make_unique<HideBotAbility>();
        break;
    case JUMP_BOT:
        jumpAbility = make_unique<JumpBotAbility>();
        break;
    default:
        break;
    }
}

void Robot::chooseSeeingUpgrade()
{
    cout << "\nSeeing upgrade for " << name << ":" << endl;
    cout << "1. ScoutBot - Can see all robots on the battlefield (3 uses)" << endl;
    cout << "2. TrackBot - Can plant trackers on robots to track their movements (3 trackers)" << endl;

    // For simulation, randomly choose
    int choice = (rand() % 2) + 1;

    switch (choice)
    {
    case 1:
        upgradeTo(SCOUT_BOT);
        cout << name << " upgraded to ScoutBot!" << endl;
        logAction("SEEING_UPGRADE: " + name + " upgraded to ScoutBot");
        break;
    case 2:
        upgradeTo(TRACK_BOT);
        cout << name << " upgraded to TrackBot!" << endl;
        logAction("SEEING_UPGRADE: " + name + " upgraded to TrackBot");
        break;
    }
}

void Robot::upgradeTo(SeeingUpgrade upgrade)
{
    seeingUpgrade = upgrade;

    switch (upgrade)
    {
    case SCOUT_BOT:
        scoutAbility = make_unique<ScoutBotAbility>();
        break;
    case TRACK_BOT:
        trackAbility = make_unique<TrackBotAbility>();
        break;
    default:
        break;
    }
}

void Robot::chooseShootingUpgrade()
{
    int choice = (rand() % 3) + 1;
    switch (choice)
    {
    case 1:
        shootingUpgrade = S_LONG_SHOT;
        cout << name << " is now a LongShotBot!\n";
        break;
    case 2:
        shootingUpgrade = S_SEMI_AUTO;
        cout << name << " is now a SemiAutoBot!\n";
        break;
    case 3:
        shootingUpgrade = S_THIRTY_SHOT;
        shells = 30;
        cout << name << " is now a ThirtyShotBot! Shells set to 30.\n";
        break;
    }
}

void Robot::chooseHealingUpgrade()
{
    if (hasHealBotUpgrade)
    {
        logAction("UPGRADE: " + name + " -> HealBot");
        return;
    }
    healAbility = make_unique<HealBotAbility>();
    lives = 5;
    hasHealBotUpgrade = true;
    cout << name << " is now a HealBot! Lives set to 5.\n";
    logAction("UPGRADE: " + name + " → HealBot (5 lives)");
}

Battlefield::Battlefield() : rows(0), cols(0), steps(0) {}

bool Battlefield::readFile(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Error opening file!" << endl;
        return false;
    }

    vector<string> lines;
    string line;
    while (getline(file, line))
        lines.push_back(line);
    file.close();

    for (const auto &line : lines)
    {
        stringstream ss(line);
        string temp;
        string type, name, rowStr, colStr;

        if (line.find("M by N") != string::npos)
        {
            ss >> temp >> temp >> temp >> temp >> rows >> cols;
            grid.resize(rows, vector<char>(cols, '.'));
            Robot::setBattlefieldSize(rows, cols);
            Robot::setBattlefield(this);
            cout << "Grid size: " << rows << "x" << cols << endl;
            logAction("SETUP: Battlefield initialized with size " + to_string(rows) + "x" + to_string(cols));
        }
        else if (line.find("steps:") != string::npos)
        {
            ss >> temp >> steps;
            cout << "Simulation steps: " << steps << endl;
            logAction("SETUP: Simulation will run for " + to_string(steps) + " steps");
        }
        else if (line.find("GenericRobot") != string::npos ||
         line.find("SniperRobot") != string::npos ||
         line.find("TankRobot") != string::npos ||
         line.find("FastRobot") != string::npos)
        {
            ss >> type >> name >> rowStr >> colStr;
            spawnRobot(type, name, rowStr, colStr);
        }
    }
    updateGrid();
    return true;
}

void Battlefield::updateGrid()
{
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            grid[i][j] = '.';
        }
    }

    for (const auto &robot : robots)
    {
        if (robot->isAlive())
        {
            int r = robot->getRow();
            int c = robot->getCol();
            if (r >= 0 && r < rows && c >= 0 && c < cols)
            {
                grid[r][c] = robot->getSymbol();
            }
        }
    }
}

void Battlefield::display() const
{
    cout << "\nCurrent Battlefield:\n";
    cout << '+';
    for (int i = 0; i < cols; i++)
        cout << '-';
    cout << "+\n";

    for (const auto &row : grid)
    {
        cout << '|';
        for (char cell : row)
            cout << cell;
        cout << "|\n";
    }

    cout << '+';
    for (int i = 0; i < cols; i++)
        cout << '-';
    cout << "+\n";

    cout << "\nActive Robots:\n";
    for (const auto &robot : robots)
    {
        if (robot->isAlive())
        {

            cout << " * " << robot->getName()
                 << " (Lives:" << robot->getLives()
                 << ", Shells: " << robot->getShells() << ")"
                 << " at (" << robot->getRow() << "," << robot->getCol() << ")\n";
        }
    }

    logAction("Battlefield:");
    // top border
    {
        string border = "+" + string(cols, '-') + "+";
        logAction(border);
    }
    // each row
    for (const auto &row : grid)
    {
        string line = "|";
        for (char cell : row)
            line += cell;
        line += "|";
        logAction(line);
    }
    // bottom border
    {
        string border = "+" + string(cols, '-') + "+";
        logAction(border);
    }
    // robot details
    logAction("Robot Details:");
}

void Battlefield::simulate()
{
    cout << "\nStarting simulation for " << steps << " turns..." << endl;
    logAction("=== SIMULATION START ===");

    bool gameEnded = false;
    for (int turn = 0; turn < steps; ++turn)
    {
        cout << "\n=== TURN " << turn + 1 << " ===" << endl;
        logAction("--- TURN " + to_string(turn + 1) + " ---");

        // 1) Remove dead robots (out of lives or shells)
        robots.erase(remove_if(robots.begin(), robots.end(),
                               [](const unique_ptr<Robot> &r)
                               {
                                   return !r->isAlive();
                               }),
                     robots.end());

        // 2) Check for no survivors
        if (robots.empty())
        {
            cout << "\nGame Over: All robots have been destroyed!" << endl;
            logAction("GAME_OVER: All robots destroyed");
            gameEnded = true;
            break;
        }

        // 3) Check for last survivor
        if (robots.size() == 1)
        {
            cout << "\nGame Over: Only one robot remains!" << endl;
            cout << robots[0]->getName() << " is the last survivor." << endl;
            logAction("GAME_OVER: Last survivor " + robots[0]->getName());
            gameEnded = true;
            break;
        }

        // 4) Otherwise, each robot takes a turn
        for (auto &robot : robots)
        {
            if (robot->isAlive())
            {
                cout << "\n[ " << robot->getName() << "'s turn ]" << endl;
                robot->performTurnActions();
            }
        }

        // 5) Update & display the grid
        updateGrid();
        display();

        cout << "\nPress Enter to continue...";
        cin.get();
    }

    // 6) If loop exited normally, max turns reached
    if (!gameEnded)
    {
        cout << "\nGame Over: Maximum turns reached (" << steps << ")." << endl;
        logAction("GAME_OVER: Max turns reached");
        cout << "Surviving robots:\n";
        for (const auto &robot : robots)
            if (robot->isAlive())
                cout << " - " << *robot << endl;
    }

    logAction("=== SIMULATION COMPLETE ===");
}

void Battlefield::spawnRobot(const string& type, const string& name, const string& rowStr, const string& colStr)
{
    int r, c;
    if (rowStr == "random" || colStr == "random")
    {
        bool conflict;
        do
        {
            conflict = false;
            r = (rowStr == "random") ? rand() % rows : stoi(rowStr);
            c = (colStr == "random") ? rand() % cols : stoi(colStr);
            r = max(0, min(r, rows - 1));
            c = max(0, min(c, cols - 1));
            for (const auto &rob : robots)
            {
                if (rob->getRow() == r && rob->getCol() == c && rob->isAlive())
                {
                    conflict = true;
                    break;
                }
            }
        } while (conflict);
    }
    else
    {
        r = max(0, min(stoi(rowStr), rows - 1));
        c = max(0, min(stoi(colStr), cols - 1));
    }

    unique_ptr<Robot> robot;

    if (type == "GenericRobot")
    {
        robot = make_unique<GenericRobot>(name, r, c);
        logAction("SPAWN: " + name + " created at position (" +
                  to_string(r) + "," + to_string(c) +
                  ") with 3 lives and 10 shells");
    }
    else if (type == "SniperRobot")
    {
        robot = make_unique<SniperRobot>(name, r, c);
        logAction("SPAWN: " + name + " created at position (" +
                  to_string(r) + "," + to_string(c) +
                  ") with 3 lives and 10 shells");
    }
    else if (type == "TankRobot")
    {
        robot = make_unique<TankRobot>(name, r, c);
        logAction("SPAWN: " + name + " created at position (" +
                  to_string(r) + "," + to_string(c) +
                  ") with 4 lives and 10 shells");
    }
    else if (type == "FastRobot")
    {
        robot = make_unique<FastRobot>(name, r, c);
        logAction("SPAWN: " + name + " created at position (" +
                  to_string(r) + "," + to_string(c) +
                  ") with 3 lives and 10 shells (fast movement)");
    }
    else
    {
        cout << "Unknown robot type: " << type << endl;
        return;
    }

    robots.push_back(move(robot));
    cout << "Created " << type << " " << name
         << " at (" << r << "," << c << ")\n";
}

Robot *Battlefield::findTargetAt(int row, int col, Robot *attacker)
{
    for (const auto &robot : robots)
    {
        if (robot.get() != attacker && robot->isAlive() &&
            robot->getRow() == row && robot->getCol() == col)
        {
            return robot.get();
        }
    }
    return nullptr;
}

bool Battlefield::positionOccupied(int r, int c)
{
    for (const auto &robot : robots)
    {
        if (robot->isAlive() && robot->getRow() == r && robot->getCol() == c)
            return true;
    }
    return false;
}

bool Battlefield::handleCombatUnified(Robot* attacker, int targetX, int targetY,
                                      const string& combatType, const string& description,
                                      HitLogicType logicType) {
    int shellsBefore = attacker->getShells();
    attacker->useShell();

    cout << attacker->getName() << " " << description << endl;
    string logMsg = combatType + ": " + attacker->getName() + " " + description;

    if (targetX < 0 || targetX >= rows || targetY < 0 || targetY >= cols) {
        cout << "  - Out of bounds." << endl;
        logAction(logMsg + " - Out of bounds.");
        return false;
    }

    Robot* target = findTargetAt(targetX, targetY, attacker);
    if (!target) {
        cout << "  - Empty space." << endl;
        logAction(logMsg + " - Empty");
        return false;
    }

    bool hit = (rand() % 10 < 7);

    if (hit) {
        cout << "  - [HIT] " << target->getName() << "!" << endl;
        logAction(logMsg + " - HIT " + target->getName());
        target->loseLives();
        attacker->markForUpgrade();

        if (!target->isAlive()) {
            cout << "[DESTROYED] " << target->getName() << "!" << endl;
        }
        return true;
    } else {
        cout << "  - [MISS] " << target->getName() << "." << endl;
        logAction(logMsg + " - MISS " + target->getName());
        return false;
    }
}

bool Battlefield::handleCombat(Robot *attacker, int targetRow, int targetCol)
{
    // Calculate direction for description
    int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    string directions[] = {"left up", "up", "right up", "left", "right", "left down", "down", "right down"};

    int dRow = targetRow - attacker->getRow();
    int dCol = targetCol - attacker->getCol();
    int direction = -1;

    for (int i = 0; i < 8; i++)
    {
        if (dx[i] == dRow && dy[i] == dCol)
        {
            direction = i;
            break;
        }
    }

    if (direction == -1)
    {
        cout << attacker->getName() << " tries to shoot at invalid target (" << targetRow << "," << targetCol << ").\n";
        return false;
    }

    string description = "shoots " + directions[direction] + " at position (" +
                        to_string(targetRow) + "," + to_string(targetCol) + ")";

    return handleCombatUnified(attacker, targetRow, targetCol, "SHOOT", description, DEFAULT_HIT_LOGIC);
}

bool Battlefield::handleSniperCombat(Robot *attacker, int dx, int dy)
{
    int targetX = attacker->getRow() + dx;
    int targetY = attacker->getCol() + dy;

    string description = "targets (" + to_string(targetX) + "," + to_string(targetY) + ")";

    return handleCombatUnified(attacker, targetX, targetY, "SNIPER_SHOT", description, SNIPER_LOGIC);
}

bool Battlefield::handleLongCombat(Robot* attacker, int dx, int dy)
{
    int tx = attacker->getRow() + dx;
    int ty = attacker->getCol() + dy;
    string description = "long-shot fires at (" + to_string(tx) + "," + to_string(ty) + ")";
    return handleCombatUnified(attacker, tx, ty, "LONG_SHOT", description, LONG_SHOT_LOGIC);
}


bool Battlefield::handleSemiAutoCombat(Robot* attacker, int dx, int dy) {
    int tx = attacker->getRow() + dx;
    int ty = attacker->getCol() + dy;

    string description = "semi-auto fires at (" + to_string(tx) + "," + to_string(ty) + ")";
    return handleCombatUnified(attacker, tx, ty, "SEMI_AUTO", description, SEMI_AUTO_LOGIC);
}

vector<pair<int, int>> Battlefield::lookAround(Robot *robot, int dx, int dy)
{
    vector<pair<int, int>> detectedEnemies;
    int centerRow = robot->getRow() + dx;
    int centerCol = robot->getCol() + dy;

    cout << robot->getName() << " looks around (" << centerRow << "," << centerCol << ")\n";

    string logMsg = "LOOK: " + robot->getName() + " looks around (" +
                    to_string(centerRow) + "," + to_string(centerCol) + ")";

    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            if (i == 0 && j == 0)
                continue;

            int r = centerRow + i;
            int c = centerCol + j;

            if (r < 0 || r >= rows || c < 0 || c >= cols)
                continue;

            Robot *target = findTargetAt(r, c, robot);
            if (target)
            {
                cout << " - Enemy found!!! Robot " << target->getName()
                     << " at (" << r << "," << c << ")" << "\n";
                detectedEnemies.emplace_back(r, c);
            }
        }
    }

    if (detectedEnemies.empty())
    {
        cout << "- No enemies detected.\n";
        logMsg += " - No enemies detected";
    }
    else
    {
        logMsg += " - Found enemies";
    }

    logAction(logMsg);
    return detectedEnemies;
}

class ThinkingRobot
{
public:
    virtual void think() = 0;
    virtual ~ThinkingRobot() = default;
};

class MovingRobot
{
public:
    virtual void move() = 0;
    virtual ~MovingRobot() = default;
};

class SeeingRobot
{
protected:
    vector<pair<int, int>> visiblePositions;

public:
    virtual void see() = 0;
    virtual ~SeeingRobot() = default;
};

class ShootingRobot
{
public:
    virtual bool shoot() = 0;
    virtual ~ShootingRobot() = default;
};

class HealingRobot
{
public:
    virtual~HealingRobot() = default;
};

bool SniperRobot::shoot()
{
    if (shells == 0)
        return false;

    if (detectedEnemyPositions.empty())
    {
        cout << "No valid targets in range. Sniper " << name << " will skip shooting." << endl;
        return false;
    }

    vector<pair<int, int>> validTargets;
    for (auto [r, c] : detectedEnemyPositions)
    {
        int distance = abs(r - row) + abs(c - col);
        if (distance <= 2 && distance > 0)
        {
            validTargets.emplace_back(r, c);
        }
    }

    if (!validTargets.empty())
    {
        auto [targetRow, targetCol] = validTargets[rand() % validTargets.size()];
        int dx = targetRow - row;
        int dy = targetCol - col;
        cout << "Sniper " << name << " targeting detected enemy at (" << targetRow << "," << targetCol << ")\n";
        return battlefield->handleSniperCombat(this, dx, dy);
    }

    cout << "No valid targets in range. Sniper " << name << " will skip shooting." << endl;
    return false;
}

void FastRobot::move()
{
    if (!isAlive()) return;

    auto [firstRow, firstCol] = findValidMovePosition();

    if (firstRow == row && firstCol == col) {
        cout << name << " stays at (" << row << "," << col << ")" << endl;
        return;
    }

    int deltaRow = firstRow - row;
    int deltaCol = firstCol - col;

    int secondRow = firstRow + deltaRow;
    int secondCol = firstCol + deltaCol;

    if (secondRow >= 0 && secondRow < battlefield->getRows() &&
        secondCol >= 0 && secondCol < battlefield->getCols() &&
        !battlefield->positionOccupied(secondRow, secondCol))
    {
        int oldRow = row, oldCol = col;
        row = secondRow;
        col = secondCol;
        cout << name << " moves 2 steps to (" << row << "," << col << ")" << endl;
        logAction("MOVE: " + name + " moved 2 steps from (" + to_string(oldRow) + "," + to_string(oldCol) +
                  ") to (" + to_string(row) + "," + to_string(col) + ")");
    }
    else
    {
        basicMove();
    }
}

int main()
{
     cout << "Welcome to Robot Battle Simulation!\n\n";
    cout << "Robot Types:\n";
    cout << "1. Generic Robot: Has 3 lives and 10 shells.\n";
    cout << "2. SniperRobot: Has 3 lives and 10 shells, but can shoot farther than other robots.\n";
    cout << "3. TankRobot: Has 4 lives and 10 shells, more durable than others.\n";
    cout << "4. FastRobot: Has 3 lives and 10 shells, and can move 1 or 2 steps per turn.\n\n";

    srand(time(nullptr));
    time_t now = time(0);
    char timeStr[100];
    strftime(timeStr, sizeof(timeStr), "%Y%m%d_%H%M%S", localtime(&now));
    string logFilename = "robot_battle_actions_" + string(timeStr) + ".log";

    logFile.open(logFilename);
    if (!logFile.is_open())
    {
        cerr << "Warning: Could not create log file " << logFilename << endl;
    }
    else
    {
        logAction("===== ROBOT BATTLE ACTION LOG =====");
        logAction("Timestamp: " + string(timeStr));
        logAction("====================================");
    }

    cout << "Robot Battle Simulation Starting..." << endl;
    cout << "Action log will be saved to: " << logFilename << endl;

    Battlefield battlefield;
    if (battlefield.readFile("setup3.txt"))
    {
        battlefield.display();
        cout << "\nPress Enter to start simulation...";
        cin.get();
        battlefield.simulate();
    }
    else
    {
        cout << "Failed to initialize battlefield." << endl;
        logAction("ERROR: Failed to initialize battlefield");
        if (logFile.is_open())
            logFile.close();
        return 1;
    }

    if (logFile.is_open())
    {
        logAction("====================================");
        logAction("===== ACTION LOG END =====");
        logFile.close();
    }

    cout << "\nSimulation completed!" << endl;
    cout << "Detailed action log saved to: " << logFilename << endl;

    return 0;
}

