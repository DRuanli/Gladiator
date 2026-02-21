#include "Fighter.h"
#include <iostream>
#include <algorithm>

Fighter::Fighter(const std::string& name, FighterType type)
    : name(name)
    , type(type)
    , defending(false)
    , poisonDamage(0)
    , poisonTurnsLeft(0){
    switch (type) {
        case FighterType::Warrior:
            maxHP = 300;
            atk = 15;
            def = 15;
            break;

        case FighterType::Mage:
            maxHP = 150;
            atk = 30;
            def = 10;
            break;

        case FighterType::Rogue:
            maxHP = 200;
            atk = 20;
            def = 20;
            break;

        case FighterType::Dragon:
            maxHP = 500;
            atk = 10;
            def = 10;
            break;

        default:
            throw  std::logic_error("Fighter type not recognized");
    }

    currentHP = maxHP;
}

int Fighter::takeDamage(int incomingDamage) {
    int effectiveDamage = defending ? incomingDamage - def : incomingDamage;
    int actualDamage = std::max(1, effectiveDamage);

    currentHP = std::max(0,currentHP - actualDamage);
    defending = false;
    return actualDamage;
}

void Fighter::heal(int amount) {
    currentHP = std::min(maxHP, currentHP + amount);
}

void Fighter::setDefending(bool d) {
    defending = d;
}

void Fighter::stealHP(int amount, Fighter& other) {
    currentHP = std::min(maxHP, currentHP + amount);
    other.currentHP = std::max(0, other.currentHP - amount);
    other.setDefending(false);
}

// --- Rouge Action ---
void Fighter::applyPoison(int damagePerTurn, int turns) {
    poisonDamage = damagePerTurn;
    poisonTurnsLeft = turns;
}

int Fighter::tickPoison() {
    if (poisonTurnsLeft <= 0) return 0;
    currentHP = std::max(0, currentHP - poisonDamage);
    poisonTurnsLeft --;
    return poisonDamage;
}

bool Fighter::isPoisoned() const {
    return poisonTurnsLeft > 0;
}

void Fighter::performSpecialAttack(Fighter &target) {
    switch (type) {
        case FighterType::Warrior: {
            int damage = atk * 2;
            target.takeDamage(damage);
        }
            break;
        case FighterType::Mage: {
            int damage = atk * 3/2;
            int reducedDef = target.def /2;
            int effectiveDamage = target.defending ? damage - reducedDef : damage;
            int actualDamage = std::max(1, effectiveDamage);
            target.currentHP = std::max(0, target.currentHP - actualDamage);
            target.defending = false;
        }
            break;

        case FighterType::Rogue:
            // Poison: 10 damage over 3 turns
            target.applyPoison(10, 3);
            break;
        default:
            throw  std::logic_error("Fighter type not recognized");
    }
}

// --- Getters ---
const std::string& Fighter::getName() const { return name; }
int Fighter::getHP() const { return currentHP; }
int Fighter::getMaxHP() const { return maxHP; }
int Fighter::getATK() const { return atk; }
int Fighter::getDEF() const { return def; }
bool Fighter::isDefending() const { return defending; }
bool Fighter::isAlive() const { return currentHP > 0; }

void Fighter::printStatus() const {
    // HP with ASCII
    int barLength = 20;
    int filledbars = (currentHP * barLength) / maxHP;

    std::cout << name << "\n";
    std::cout << "  HP: [";
    for (int i = 0; i < barLength; ++i)
        std::cout << (i < filledbars ? '#' : '-');
    std::cout << "] " << currentHP << "/" << maxHP << "\n";
    std::cout << "  ATK: " << atk
              << "  DEF: " << def;
    if (defending)
        std::cout << "  [DEFENDING]";
    if (isPoisoned())
        std::cout << "  [POISONED: " << poisonTurnsLeft << " turns]";
    std::cout << "\n";
}

