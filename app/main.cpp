/**
 * @file main.cpp
 * @brief Console front end of the vending machine.
 *
 * Reads commands from the keyboard and turns them into the machine's
 * events. All the logic is in the library; this file only translates
 * text <-> events and plays the part of the hardware:
 *
 * - "coins dispensed": paying out is instant here, so main calls
 *   coinsDispensed() as soon as the machine is in Dispensing;
 * - "brew done / brew failed": while the machine is Brewing, Enter means
 *   the drink is ready, 'fail' simulates a breakdown. Anything else is
 *   answered with "busy", so the user always knows what is going on.
 */
#include "vending/vending_machine.hpp"

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>

using namespace vending;

namespace {

/// Coins the coin acceptor takes. Anything else is rejected before the machine.
const std::set<int> kValidCoins = {1, 2, 5, 10};

/// What a technician does in the morning: menu, cash, ingredients.
void prepareMachine(VendingMachine& machine)
{
    machine.menu().addDrink({"Espresso", 7,
                             {{Ingredient::Coffee, 8}, {Ingredient::Water, 30}}});
    machine.menu().addDrink({"Americano", 8,
                             {{Ingredient::Coffee, 8}, {Ingredient::Water, 120}}});
    machine.menu().addDrink({"Latte", 10,
                             {{Ingredient::Coffee, 8}, {Ingredient::Water, 30},
                              {Ingredient::Milk, 100}}});

    machine.coins().loadCoins(1, 5);
    machine.coins().loadCoins(2, 5);
    machine.coins().loadCoins(5, 2);

    machine.brewer().loadIngredient(Ingredient::Coffee, 50);   // 6 cups
    machine.brewer().loadIngredient(Ingredient::Water, 1000);
    machine.brewer().loadIngredient(Ingredient::Milk, 300);    // 3 lattes
}

const char* stateName(StateId id)
{
    switch (id)
    {
    case StateId::Idle:       return "Idle";
    case StateId::HasMoney:   return "HasMoney";
    case StateId::Brewing:    return "Brewing";
    case StateId::Dispensing: return "Dispensing";
    case StateId::OutOfOrder: return "OutOfOrder";
    }
    return "?";
}

void printHelp()
{
    std::cout << "Commands:\n"
                 "  menu       show the drinks\n"
                 "  coin N     insert a coin (1, 2, 5, 10)\n"
                 "  drink N    press the button of drink N\n"
                 "  cancel     press Cancel\n"
                 "  help       this text\n"
                 "  quit       exit\n";
}

/// "5 + 2 + 2": the coins of a payout, largest first.
std::string coinList(const std::map<int, int>& coins)
{
    std::string text;
    for (auto it = coins.rbegin(); it != coins.rend(); ++it)
    {
        const auto& [value, count] = *it;
        for (int i = 0; i < count; ++i)
        {
            if (!text.empty())
                text += " + ";
            text += std::to_string(value);
        }
    }
    return text;
}

/// Shows what the machine has just paid out and why.
void printPayout(VendingMachine& machine, const char* what, int amount)
{
    std::cout << what << ": " << amount;
    if (amount > 0)
        std::cout << "   (coins: " << coinList(machine.coins().lastPayout()) << ")";
    std::cout << "\n";
}

void printStatus(VendingMachine& machine)
{
    std::cout << "Balance: " << machine.coins().balance()
              << "   [" << stateName(machine.state()) << "]\n";
}

/**
 * @brief Plays the brewer: Enter = the drink is ready, 'fail' = breakdown.
 * @param drinkName shown to the user while brewing
 * @return false if the input ended.
 */
bool finishBrewing(VendingMachine& machine, const std::string& drinkName)
{
    std::cout << "Brewing " << drinkName << "...\n"
              << "(simulation: press Enter when the drink is ready,"
                 " or type 'fail' to simulate a breakdown)\n";

    std::string line;
    while (std::cout << "> " && std::getline(std::cin, line))
    {
        if (line.empty() || line == "done") { machine.brewDone();   return true; }
        if (line == "fail")                 { machine.brewFailed(); return true; }
        std::cout << "The machine is busy brewing. Press Enter, or type 'fail'.\n";
    }
    return false;
}

} // namespace

int main()
{
    VendingMachine machine;   // the UI writes to std::cout
    prepareMachine(machine);

    std::cout << "Coffee vending machine\n";
    machine.ui().showMenu(machine.menu());
    printHelp();

    std::string line;
    while (std::cout << "> " && std::getline(std::cin, line))
    {
        std::istringstream in(line);
        std::string command;
        in >> command;

        DrinkId selected = 0;   // set by "drink N"

        // Remembered before the event: after it, the money is already paid out.
        const int dueBefore     = machine.coins().changeDue();
        const int balanceBefore = machine.coins().balance();

        if (command == "quit")
            break;
        else if (command.empty())
            continue;
        else if (command == "help")
            printHelp();
        else if (command == "menu")
            machine.ui().showMenu(machine.menu());
        else if (command == "cancel")
            machine.cancel();
        else if (command == "coin")
        {
            int value = 0;
            if (in >> value && kValidCoins.count(value) != 0)
            {
                std::cout << "Coin accepted: " << value << "\n";
                machine.insertCoin(value);
            }
            else
                std::cout << "Coin rejected\n";
        }
        else if (command == "drink")
        {
            int number = 0;
            if (in >> number && number >= 1
                && static_cast<std::size_t>(number) <= machine.menu().size())
            {
                selected = static_cast<DrinkId>(number - 1);   // button 1 = DrinkId 0
                machine.selectDrink(selected);
            }
            else
                std::cout << "No such drink\n";
        }
        else
        {
            std::cout << "Unknown command, type 'help'\n";
            continue;
        }

        // The machine started brewing: play the brewer.
        // Only "drink N" can lead here, so selected is the drink being brewed.
        int due = dueBefore;
        if (machine.state() == StateId::Brewing)
        {
            due = machine.coins().changeDue();   // the change of this purchase
            if (!finishBrewing(machine, machine.menu().drink(selected).name))
                break;
        }

        // Paying out is instant here: report it and send "coins dispensed".
        // After a purchase it is change, after Cancel it is a refund.
        if (machine.state() == StateId::Dispensing)
        {
            printPayout(machine, command == "cancel" ? "Refund" : "Change", due);
            machine.coinsDispensed();
        }

        if (machine.state() == StateId::OutOfOrder)
        {
            printPayout(machine, "Refund", balanceBefore);
            std::cout << "The machine needs a technician. Bye.\n";
            break;
        }

        printStatus(machine);
    }
    return 0;
}
