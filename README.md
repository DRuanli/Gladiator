# Project 0.1 — Number Gladiator: Hướng dẫn từng bước

---

## 1. Setup môi trường (CL)

### Kiểm tra compiler

```bash
g++ --version
```

Nếu chưa có:
- **macOS**: `xcode-select --install`
- **Linux (Ubuntu/Debian)**: `sudo apt install g++ build-essential`
- **Windows**: Cài [MSYS2](https://www.msys2.org/) → `pacman -S mingw-w64-ucrt-x86_64-gcc`

### Tạo folder structure

```bash
mkdir number-gladiator
cd number-gladiator
mkdir src
touch src/main.cpp src/Fighter.h src/Fighter.cpp
```

Kết quả:
```
number-gladiator/
└── src/
    ├── main.cpp
    ├── Fighter.h
    └── Fighter.cpp
```

---

## 2. Khái niệm cốt lõi cần hiểu TRƯỚC khi code

### 2.1 Tại sao phải tách `.h` và `.cpp`?

Trong Java bạn quen viết:
```java
// Fighter.java — declaration + implementation cùng 1 file
public class Fighter {
    private String name;
    public void attack() { ... }  // implement ngay tại đây
}
```

Trong C++, tách đôi:
```
Fighter.h   → "Hợp đồng": class này CÓ GÌ (declaration)
Fighter.cpp → "Thực hiện": class này LÀM GÌ (implementation)
main.cpp    → #include "Fighter.h" để biết Fighter là gì,
              linker sẽ tìm Fighter.cpp để lấy implementation
```

**Lý do thực tế**: Nếu bạn có 10 files dùng `Fighter`, chúng chỉ cần `#include "Fighter.h"`. Compiler compile mỗi `.cpp` **một lần**, linker ghép lại. Không phải compile Fighter 10 lần.

### 2.2 Compilation pipeline (thứ Java ẩn hoàn toàn)

```
Khi bạn chạy: g++ src/main.cpp src/Fighter.cpp -o game

Bước 1 — PREPROCESSOR:
  #include "Fighter.h" → copy toàn bộ nội dung Fighter.h vào

Bước 2 — COMPILER:
  main.cpp  → main.o   (object file)
  Fighter.cpp → Fighter.o (object file)

Bước 3 — LINKER:
  main.o + Fighter.o → game (executable)
  Linker tìm: "Fighter::attack() được khai báo trong .h,
               implementation nằm đâu?" → Fighter.o có nó
```

### 2.3 `const` method — thứ Java không có

```cpp
int getHP() const;   // "const" sau () = method này KHÔNG thay đổi object
void takeDamage(int amount);  // không có const = có thể thay đổi object
```

Java không cần vì mọi method đều có thể modify object. C++ bắt bạn **cam kết** explicitly.

### 2.4 `#pragma once` — thay vì header guard

```cpp
// Đặt dòng này ở ĐẦU mọi .h file
#pragma once

// Tác dụng: nếu Fighter.h bị #include nhiều lần trong cùng translation unit
// → compiler chỉ đọc 1 lần, không bị "redefinition error"
```

---

## 3. Build từng bước

### Bước 1: Viết `Fighter.h` — Declaration

```cpp
// src/Fighter.h
#pragma once

#include <string>

class Fighter
{
public:
    // Constructor — nhận name, hp, atk, def
    Fighter(const std::string& name, int hp, int atk, int def);

    // --- Actions ---
    // Trả về damage thực sự đã nhận (sau khi trừ def)
    int takeDamage(int incomingDamage);
    void heal(int amount);
    void setDefending(bool defending);

    // --- Getters (tất cả đều const vì không thay đổi object) ---
    const std::string& getName() const;
    int getHP() const;
    int getMaxHP() const;
    int getATK() const;
    int getDEF() const;
    bool isDefending() const;
    bool isAlive() const;

    // --- Display ---
    void printStatus() const;

private:
    std::string name;
    int currentHP;
    int maxHP;
    int atk;
    int def;
    bool defending;
};
```

> **Lưu ý Java dev**: Trong Java bạn viết `private String name;` trong class body cùng lúc với implement methods. Ở đây bạn chỉ **khai báo** — chưa có code gì bên trong.

---

### Bước 2: Viết `Fighter.cpp` — Implementation

```cpp
// src/Fighter.cpp
#include "Fighter.h"
#include <iostream>
#include <algorithm>   // std::max, std::min

// Constructor với initializer list (C++ specific)
// Thay vì: this.name = name trong body (Java style)
// C++ dùng: `: name(n), currentHP(hp)` — initialize trực tiếp
Fighter::Fighter(const std::string& name, int hp, int atk, int def)
    : name(name)
    , currentHP(hp)
    , maxHP(hp)
    , atk(atk)
    , def(def)
    , defending(false)
{
    // Body có thể trống — initializer list đã lo hết
}

int Fighter::takeDamage(int incomingDamage)
{
    // Nếu đang defend → chỉ nhận 40% damage, round xuống
    int effectiveDamage = defending ? incomingDamage * 4 / 10 : incomingDamage;

    // Damage giảm bởi def, tối thiểu 1 (không bao giờ heal từ attack)
    int actualDamage = std::max(1, effectiveDamage - def);

    currentHP = std::max(0, currentHP - actualDamage);
    defending = false;   // reset sau mỗi turn

    return actualDamage;
}

void Fighter::heal(int amount)
{
    // std::min để không vượt maxHP
    currentHP = std::min(maxHP, currentHP + amount);
    defending = false;
}

void Fighter::setDefending(bool d)
{
    defending = d;
}

// --- Getters ---
const std::string& Fighter::getName() const { return name; }
int Fighter::getHP()                  const { return currentHP; }
int Fighter::getMaxHP()               const { return maxHP; }
int Fighter::getATK()                 const { return atk; }
int Fighter::getDEF()                 const { return def; }
bool Fighter::isDefending()           const { return defending; }
bool Fighter::isAlive()               const { return currentHP > 0; }

void Fighter::printStatus() const
{
    // Thanh HP bằng ký tự ASCII
    int barLength   = 20;
    int filledBars  = (currentHP * barLength) / maxHP;

    std::cout << name << "\n";
    std::cout << "  HP: [";
    for (int i = 0; i < barLength; ++i)
        std::cout << (i < filledBars ? '#' : '-');
    std::cout << "] " << currentHP << "/" << maxHP << "\n";
    std::cout << "  ATK: " << atk
              << "  DEF: " << def;
    if (defending)
        std::cout << "  [DEFENDING]";
    std::cout << "\n";
}
```

> **Syntax quan trọng**: `Fighter::takeDamage` — dùng `::` (scope resolution operator) để nói "đây là method của class Fighter". Trong Java không cần vì mọi thứ đã nằm trong class block.

---

### Bước 3: Viết `main.cpp` — Game Loop

```cpp
// src/main.cpp
#include "Fighter.h"
#include <iostream>
#include <random>
#include <string>
#include <limits>

// --- Random number helper ---
// Trả về số ngẫu nhiên trong [min, max] (inclusive)
int randomInt(int min, int max)
{
    // C++ random: cần engine + distribution
    // static để không tạo mới mỗi lần gọi (giống Java Random instance)
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

// --- Display separator ---
void printLine()
{
    std::cout << "========================================\n";
}

// --- Player turn: trả về action string ---
std::string getPlayerAction()
{
    std::cout << "\nChoose action:\n";
    std::cout << "  1. Attack\n";
    std::cout << "  2. Defend (take 40% damage next turn)\n";
    std::cout << "  3. Heal (+20 HP)\n";
    std::cout << "  4. Heavy Strike (150% ATK, skip defending)\n";
    std::cout << "> ";

    int choice = 0;
    std::cin >> choice;

    // Xử lý input không hợp lệ
    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return "invalid";
    }

    switch (choice)
    {
        case 1: return "attack";
        case 2: return "defend";
        case 3: return "heal";
        case 4: return "heavy";
        default: return "invalid";
    }
}

// --- Resolve 1 turn ---
// player thực hiện action, enemy phản công
void resolveTurn(Fighter& player, Fighter& enemy, const std::string& action)
{
    printLine();

    // --- Player action ---
    if (action == "attack")
    {
        // Damage = ATK ± 20% random variation
        int base   = player.getATK();
        int damage = randomInt(base * 8 / 10, base * 12 / 10);
        int actual = enemy.takeDamage(damage);
        std::cout << player.getName() << " attacks for " << actual << " damage!\n";
    }
    else if (action == "defend")
    {
        player.setDefending(true);
        std::cout << player.getName() << " takes a defensive stance!\n";
    }
    else if (action == "heal")
    {
        player.heal(20);
        std::cout << player.getName() << " heals for 20 HP!\n";
    }
    else if (action == "heavy")
    {
        int base   = player.getATK() * 3 / 2;   // 150%
        int damage = randomInt(base * 8 / 10, base * 12 / 10);
        int actual = enemy.takeDamage(damage);
        std::cout << player.getName() << " unleashes a HEAVY STRIKE for " << actual << " damage!\n";
    }
    else
    {
        std::cout << "Invalid action — turn wasted!\n";
    }

    // --- Enemy counterattack (nếu vẫn sống) ---
    if (enemy.isAlive())
    {
        // AI đơn giản: 70% attack, 20% defend, 10% heal
        int roll = randomInt(1, 10);
        if (roll <= 7)
        {
            int base   = enemy.getATK();
            int damage = randomInt(base * 8 / 10, base * 12 / 10);
            int actual = player.takeDamage(damage);
            std::cout << enemy.getName() << " attacks back for " << actual << " damage!\n";
        }
        else if (roll <= 9)
        {
            enemy.setDefending(true);
            std::cout << enemy.getName() << " prepares to defend!\n";
        }
        else
        {
            enemy.heal(15);
            std::cout << enemy.getName() << " heals for 15 HP!\n";
        }
    }
}

// --- Main ---
int main()
{
    std::cout << "=== NUMBER GLADIATOR ===\n\n";
    std::cout << "Enter your fighter's name: ";
    std::string playerName;
    std::getline(std::cin, playerName);
    if (playerName.empty()) playerName = "Hero";

    // Tạo fighters trên STACK (không cần new!)
    // Khi main() kết thúc → tự destroy
    Fighter player(playerName, 100, 18, 6);
    Fighter enemy("Shadow Mage", 80, 22, 3);

    std::cout << "\n--- BATTLE START ---\n";

    int turn = 1;

    while (player.isAlive() && enemy.isAlive())
    {
        std::cout << "\n=== TURN " << turn++ << " ===\n";
        player.printStatus();
        std::cout << "\n";
        enemy.printStatus();

        std::string action = getPlayerAction();
        resolveTurn(player, enemy, action);
    }

    // --- Result ---
    printLine();
    if (player.isAlive())
        std::cout << "\n*** " << player.getName() << " WINS! ***\n";
    else
        std::cout << "\n*** " << enemy.getName() << " wins. You were defeated. ***\n";

    return 0;
}
```

---

## 4. Compile và chạy

```bash
# Từ folder number-gladiator/
g++ src/main.cpp src/Fighter.cpp -o game -std=c++17 -Wall -Wextra

# Chạy:
./game          # macOS/Linux
game.exe        # Windows
```

**Giải thích flags:**

| Flag | Tác dụng |
|---|---|
| `-std=c++17` | Dùng C++17 standard (cần cho một số features) |
| `-Wall` | Bật mọi warnings thông thường |
| `-Wextra` | Bật thêm warnings nâng cao |
| `-o game` | Đặt tên output là `game` |

> **Lưu ý**: Nếu bạn chỉ viết `g++ main.cpp` mà thiếu `Fighter.cpp` → **linker error**: *undefined reference to Fighter::Fighter*. Đây là lỗi bạn SẼ gặp ít nhất một lần — rất bình thường, và đó là cách học compilation model.

---

## 5. Lỗi thường gặp & cách debug

| Lỗi | Nguyên nhân | Fix |
|---|---|---|
| `undefined reference to Fighter::...` | Quên thêm `Fighter.cpp` vào compile command | Thêm `src/Fighter.cpp` vào lệnh g++ |
| `Fighter was not declared in this scope` | Quên `#include "Fighter.h"` trong main.cpp | Thêm include |
| `error: redefinition of class Fighter` | Include Fighter.h nhiều hơn 1 lần mà thiếu `#pragma once` | Thêm `#pragma once` đầu Fighter.h |
| `passing X as 'this' discards qualifiers` | Gọi non-const method trên const object | Thêm `const` vào method declaration + definition |

---

## 6. Bonus challenges (khi done)

Làm từng cái theo thứ tự — mỗi cái thêm 1 concept:

**Bonus 1**: Thêm `enum class FighterType { Warrior, Mage, Rogue }` — mỗi type có stats khác nhau khi khởi tạo. Học: `enum class` syntax và switch on enum.

**Bonus 2**: Cho player chọn fighter type ở đầu game. Học: input handling phức tạp hơn.

**Bonus 3**: Thêm "Special Attack" riêng cho từng type — Warrior: Slam (high damage), Mage: Fireball (ignore 50% DEF), Rogue: Poison (damage over 3 turns). Học: lưu state qua nhiều turns.

**Bonus 4**: Thêm second enemy file `Boss.h / Boss.cpp` kế thừa Fighter, override `takeDamage()` để Boss có passive resistance. Học: **inheritance + override** trong C++ — đây là bridge trực tiếp sang UE5 Actor hierarchy.

---

Bắt đầu với `Fighter.h` trước — chỉ cần viết phần declaration. Khi compile lần đầu và gặp lỗi, paste lỗi vào đây mình sẽ giải thích cụ thể.