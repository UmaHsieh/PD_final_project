#include <iostream>
#include <utility> // For pair
#include <vector>  // For vector
#include <string>
#include <chrono>
#include <thread>
using namespace std;

const int MAP_ROWS = 10;
const int MAP_COLS = 10;

// 一個簡單的二維座標結構
struct Position {
    int x;
    int y;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
};

class Character 
{
protected:
    string name; 
    int health;
    Position position; // 使用Position結構來代替pair
public:
    Character(const string& n, int hp, int x, int y) 
    : name(n), health(hp), position(x, y) {}
    string getName() const {return name;}
    int getHealth () const {return health;}
    // 更新位置的方法
    void changeLocation(int x, int y) {
        position.x = x;
        position.y = y;
    }
    // 獲取位置的方法
    Position getPosition() const {return position;}

    virtual void attack(Character& target) = 0;
};

class Player : public Character
{
private:
    int level;
    int experience;
    int specialNumber; // 特殊編號，對應道具的數字
public:
    Player(const string& n, int x, int y, int specialNum)
    : Character(n, 100, x, y), level(0), experience(0), specialNumber(specialNum) {}
    int getExperience () const {return experience;}
    void levelUp(int gainedExperience) {
        experience += gainedExperience; // 添加獲得的經驗值到當前經驗值
        while (experience >= 100) 
        {  // 當經驗值足夠升級時，進入循環
            level++; // 升一級
            experience -= 100; // 減去升級所需的經驗值
            health += 1; // 假設每升一級，增加1點生命值
            // 可以在這裡添加其他升級邏輯，如增加攻擊力、防禦力等
        }
    }
    void attack(Character& target) override {
        // Perform player's attack logic
    }
};

class Enemy : public Character 
{
public:
    Enemy(const string& n, int hp, int x, int y) : Character(n, hp, x, y) {}
    void attack(Character& target) override {
        // Perform enemy's attack logic
    } 
};

class Wall {
private:
    Position position;
public:
    Wall(int x, int y) : position(x, y) {}
    Position getPosition() const {return position;}
};

class Item 
{
private:
    Position position;
    string name;
    int specialNumber; // 特殊編號
public:
    Item(const string& n, int x, int y, int specialNum) : name(n), position(x, y), specialNumber(specialNum) {}
    string getName() const {return name;}
    int getSpecialNumber() const {return specialNumber;}
};

class Game 
{
private:
    Player player;
    vector<Enemy> enemies;
    vector<Item> items;
    vector<Wall> walls;
public:
    Game(const string& playerName, int playerX, int playerY, int playerSpecialNumber)
    : player(playerName, playerX, playerY, playerSpecialNumber) {}

    void addEnemy(const Enemy& enemy) {enemies.push_back(enemy);}
    void addItem(const Item& item) {items.push_back(item);}
    void addWall(const Wall& wall) {walls.push_back(wall);}
    // Game logic methods
};

void initializeGameMap(vector<vector<char> >& map) {
    map.resize(MAP_ROWS, vector<char>(MAP_COLS, '0')); // 以'0'填充代表空地
    // 可以在這裡放置牆壁和其他物件
}

// 繪製遊戲界面
void drawGame(const string& playerName, int totalEnemyHealth, const vector<vector<char> >& map) {
    system("clear"); // 清屏，對於Windows使用system("cls");
    
    // 繪製名字和計時器在右上角
    cout << string(50, ' ') << "Player: " << playerName << " Time: " << "00:00" << endl;
    
    // 繪製 totalEnemyHealth 在左上角
    cout << "Total Enemy Health: " << totalEnemyHealth << endl;
    
    // 繪製遊戲地圖
    for (const auto& row : map) {
        for (char cell : row) {
            cout << cell << ' ';
        }
        cout << endl;
    }
}

void movePlayer(char direction, vector<vector<char> >& map, Player& player, int& totalEnemyHealth) {
    Position playerPos = player.getPosition();
    int newX = playerPos.x;
    int newY = playerPos.y;

    // 確定新的位置
    switch (direction) {
        case 'W': newY--; break; // 向上移動
        case 'A': newX--; break; // 向左移動
        case 'S': newY++; break; // 向下移動
        case 'D': newX++; break; // 向右移動
    }

    // 檢查新位置是否超出地圖範圍
    if (newX < 0 || newX >= MAP_COLS || newY < 0 || newY >= MAP_ROWS) {
        return; // 如果超出範圍，不進行移動
    }

    // 根據物件執行動作
    switch (map[newY][newX]) {
        case '0': // 空地，可以移動
            map[playerPos.y][playerPos.x] = '0'; // 清除舊位置
            player.changeLocation(newX, newY); // 更新玩家位置
            map[newY][newX] = 'P'; // 在新位置放置玩家
            break;
        case 'W': // 牆，不移動
            break;
        case 'F': // 火，進行戰鬥
            totalEnemyHealth -= 10; // 減少敵人總血量
            if (totalEnemyHealth <= 0) {
                map[newY][newX] = '.'; // 清除敵人位置
            }
            // 在這裡處理與火的戰鬥邏輯
            // 減少totalEnemyHealth，如果冰或火血量歸零，更新地圖
            break;
        case 'I': // 冰，可以移動，可能還需要交互
            // 假設移動冰塊，冰塊會滑到下一格，除非下一格是牆壁
            int nextX = newX, nextY = newY;
            switch (direction) {
                case 'W': nextY--; break;
                case 'A': nextX--; break;
                case 'S': nextY++; break;
                case 'D': nextX++; break;
            }
            if (nextX >= 0 && nextX < MAP_COLS && nextY >= 0 && nextY < MAP_ROWS 
                && map[nextY][nextX] == '.') {
                map[newY][newX] = '.'; // 清除原來的冰塊
                map[nextY][nextX] = 'I'; // 冰塊滑到新位置
            }
            // 玩家移動到冰塊原位置
            map[playerPos.y][playerPos.x] = '.'; // 清除舊位置
            player.changeLocation(newX, newY); // 更新玩家位置
            map[newY][newX] = 'P'; // 玩家移動到冰塊位置
            break;
            // 在這裡處理與冰的交互邏輯
        // ... 其他情況 ...
    }
}

int main()
{
    string name;
    cout << "請輸入名字: ";
    cin >> name;

    // 初始化遊戲地圖
    vector<vector<char> > gameMap;
    initializeGameMap(gameMap);

    // 初始化玩家位置和特殊編號
    int playerX = 0; // 需要根據遊戲設計來指定初始值
    int playerY = 0; // 需要根據遊戲設計來指定初始值
    int playerSpecialNumber = 1; // 假設的初始特殊編號

    // 創建遊戲實例
    Player player(name, 0, 0, 1); // 假設玩家初始位置在(0, 0)
    // Game game(name, 0, 0, 1); // 假設玩家初始位置為(0,0)，特殊編號為1

    // 加入敵人、物品和牆壁等遊戲對象
    // 這裡需要根據每個關卡的設計來動態添加
    // game.addEnemy(Enemy("Fire", 10, 1, 1));
    // game.addItem(Item("Ice Shard", 2, 2, 5));
    // game.addWall(Wall(3, 3));

    // 遊戲主循環
    int totalEnemyHealth = 100; // 假設的敵人總血量
    bool gameOver = false;
    
    while (!gameOver) {
        char input;
        cout << "請輸入移動指令 (WASD): ";
        cin >> input;

        // 根據輸入更新遊戲狀態
        switch (input) {
            case 'W': // 向上移動
                // 更新玩家位置
                // 移動玩家
                movePlayer(input, gameMap, player, totalEnemyHealth);
                // 檢查是否過關
                if (totalEnemyHealth <= 0) {
                    cout << "Congratulations! You've completed the level." << endl;
                    // 進入下一關或結束遊戲
                    gameOver = true; // 假設遊戲結束
                }
                break;
            case 'A': // 向左移動
                // 更新玩家位置
                break;
            case 'S': // 向下移動
                // 更新玩家位置
                break;
            case 'D': // 向右移動
                // 更新玩家位置
                break;
            case 'E': // 特殊動作，例如攻擊
                // 執行攻擊邏輯
                break;
            case 'Q': // 離開遊戲
                gameOver = true;
                break;
            default:
                cout << "無效的輸入!" << endl;
                break;
        }

        this_thread::sleep_for(chrono::milliseconds(500));
    }
    // 遊戲結束處理...
    cout << "遊戲結束，" << name << " 的得分是: " << /* 玩家得分 */ endl;
    // ...其他結算邏輯

    return 0;
}
