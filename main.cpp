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

Fighter::FighterType getPlayerRole()
{
    while (true)
    {
        std::cout << "\nChoose your role:\n";
        std::cout << "  1. Warrior (Tanky)\n";
        std::cout << "  2. Mage (High damage, low HP)\n";
        std::cout << "  3. Rogue (Balanced)\n";
        std::cout << "  4. Dragon\n";
        std::cout << "> ";

        int choice;
        std::cin >> choice;

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Enter a number.\n";
            continue;
        }

        switch (choice)
        {
            case 1: return Fighter::FighterType::Warrior;
            case 2: return Fighter::FighterType::Mage;
            case 3: return Fighter::FighterType::Rogue;
            case 4: return Fighter::FighterType::Dragon;
            default:
                std::cout << "Invalid choice. Please select 1-4.\n";
        }
    }
}

// --- Player turn: trả về action string ---
std::string getPlayerAction()
{
    std::cout << "\nChoose action:\n";
    std::cout << "  1. Attack\n";
    std::cout << "  2. Defend (reduce damage by DEF next turn)\n";
    std::cout << "  3. Heal (+20 HP)\n";
    std::cout << "  4. Heavy Strike (150% ATK, skip defending)\n";
    std::cout << "  5. Steal HP\n";
    std::cout << "  6. Special Attack\n";
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
        case 5: return "steal";
        case 6: return "special";
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
        player.setDefending(false);

        std::cout << player.getName() << " unleashes a HEAVY STRIKE for " << actual << " damage!\n";
    }
    else if (action == "steal") {
        int base = player.getATK();
        int stealAmount = randomInt(base * 3/10, base * 5/10);
        player.stealHP(stealAmount, enemy);
        std::cout << player.getName() << " steals " << stealAmount << " HP!\n";
    }
    else if (action == "special") {
        player.setDefending(false);
        player.performSpecialAttack(enemy);
        std::cout << player.getName() << " uses SPECIAL ATTACK!\n";
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
        else if (roll <= 8)
        {
            enemy.setDefending(true);
            std::cout << enemy.getName() << " prepares to defend!\n";
        }
        else if (roll <= 9) {
            int a = enemy.getATK();
            int s = randomInt(a*3/10, a*5/10);
            enemy.stealHP(s, player);
            std::cout << enemy.getName() << " steals " << s << " HP!\n";
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

    Fighter::FighterType role = getPlayerRole();
    Fighter player(playerName, role);
    // Tạo fighters t1rên STACK (không cần new!)
    Fighter enemy("Shadow", Fighter::FighterType::Dragon);

    std::cout << "\n--- BATTLE START ---\n";

    int turn = 1;

    while (player.isAlive() && enemy.isAlive())
    {
        std::cout << "\n=== TURN " << turn++ << " ===\n";
        player.printStatus();
        std::cout << "\n";
        enemy.printStatus();

        // Apply poison damage at start of turn
        int poisonDmg = player.tickPoison();
        if (poisonDmg > 0) {
            std::cout << "\n" << player.getName() << " takes " << poisonDmg << " poison damage!\n";
        }

        poisonDmg = enemy.tickPoison();
        if (poisonDmg > 0) {
            std::cout << enemy.getName() << " takes " << poisonDmg << " poison damage!\n";
        }

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