#pragma once

#include <string>

class Fighter {
public:
    enum class FighterType {
        Warrior,
        Mage,
        Rogue,
        Dragon
    };
    Fighter(const std::string& name, FighterType type);

    // --- Action ---
    int takeDamage(int incomingDamage);
    void heal(int amount);
    void setDefending(bool defending);
    void stealHP(int amount, Fighter& other);

    // -- Rouge actions ---
    void applyPoison(int damagePerTurn, int turns);
    int tickPoison();
    bool isPoisoned() const;
    void performSpecialAttack(Fighter& target);

    // --- Getters ---
    const std::string& getName() const;
    int getHP() const;
    int getMaxHP() const;
    int getATK() const;
    int getDEF() const;
    bool isDefending() const;
    bool isAlive() const;

    void printStatus() const;

private:
    std::string name;
    FighterType type;

    int currentHP;
    int maxHP;
    int atk;
    int def;
    bool defending;

    // --- Special for Rouge
    int poisonDamage;
    int poisonTurnsLeft;
};