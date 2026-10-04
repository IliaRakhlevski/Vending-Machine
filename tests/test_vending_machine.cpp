/**
 * @file test_vending_machine.cpp
 * @brief Tests of VendingMachine. Returns 0 when every check passes.
 *
 * The scenario tests drive the machine only through its public events
 * (insertCoin, selectDrink, cancel, brewDone, brewFailed, coinsDispensed)
 * and check what is visible from outside: the state, the balance, the
 * stock of the brewer and the text the UI has shown. The UI writes into a
 * std::ostringstream, so a test reads the "screen" as a string.
 *
 * Row numbers refer to the transition table in README.md.
 */
#include "vending/vending_machine.hpp"

#include <iostream>
#include <sstream>
#include <string>

using namespace vending;

namespace {

/// Prints the message if the condition is false. Returns the condition,
/// so checks can be chained with &&: the first failed one stops the chain.
bool expect(bool condition, const char* message)
{
    if (!condition)
        std::cout << "FAILED: " << message << std::endl;
    return condition;
}

/// True if the UI has shown @p text somewhere.
bool screenHas(const std::ostringstream& screen, const std::string& text)
{
    return screen.str().find(text) != std::string::npos;
}

/// Espresso, price 7: 8 g coffee, 30 ml water. Gets DrinkId 0.
const Drink kEspresso{"Espresso", 7, {{Ingredient::Coffee, 8}, {Ingredient::Water, 30}}};

/// Enough ingredients for several espressos.
void loadIngredients(VendingMachine& vm)
{
    vm.brewer().loadIngredient(Ingredient::Coffee, 100);
    vm.brewer().loadIngredient(Ingredient::Water, 1000);
}

} // namespace

// ------------------------------------------------------------ basic

/// A new machine waits with balance 0.
bool testInitialStateIsIdle()
{
    VendingMachine vm;

    if (vm.state() == StateId::Idle)
        return true;
    std::cout << "FAILED: initial state is not Idle" << std::endl;
    return false;
}

/// After setState(next) the machine is in next.
bool testSetStateSwitches()
{
    VendingMachine vm;

    vm.setState(vm.brewing());

    if (vm.state() == StateId::Brewing)
        return true;
    std::cout << "FAILED: setState(brewing) did not switch to Brewing" << std::endl;
    return false;
}

// ------------------------------------------------------------ scenarios

/// Row 1: the first coin moves the machine from Idle to HasMoney.
bool testCoinInIdleGoesToHasMoney()
{
    std::ostringstream screen;
    VendingMachine vm(screen);

    vm.insertCoin(5);

    return expect(vm.state() == StateId::HasMoney, "coin in Idle: state is not HasMoney")
        && expect(vm.coins().balance() == 5,       "coin in Idle: balance is not 5");
}

/// Row 2: a button pressed with no money only shows the price.
bool testButtonWithoutMoneyShowsPrice()
{
    std::ostringstream screen;
    VendingMachine vm(screen);
    vm.menu().addDrink(kEspresso);

    vm.selectDrink(0);

    return expect(vm.state() == StateId::Idle,  "button in Idle: state changed")
        && expect(screenHas(screen, "Price: 7"), "button in Idle: price not shown");
}

/// Rows 5 and 13: cancel returns the customer's money and the machine
/// comes back to Idle once the coins are out.
bool testCancelReturnsMoney()
{
    std::ostringstream screen;
    VendingMachine vm(screen);

    vm.insertCoin(5);
    vm.insertCoin(2);
    vm.cancel();

    const bool paidOut = expect(vm.state() == StateId::Dispensing, "cancel: state is not Dispensing")
                      && expect(vm.coins().balance() == 0,         "cancel: balance is not 0");
    if (!paidOut)
        return false;

    vm.coinsDispensed();
    return expect(vm.state() == StateId::Idle, "cancel: not back to Idle after coinsDispensed");
}

/// Row 6: not enough money - the price is shown, the money stays.
bool testNotEnoughMoneyShowsPrice()
{
    std::ostringstream screen;
    VendingMachine vm(screen);
    vm.menu().addDrink(kEspresso);
    loadIngredients(vm);

    vm.insertCoin(5);
    vm.selectDrink(0);

    return expect(vm.state() == StateId::HasMoney, "not enough money: state changed")
        && expect(vm.coins().balance() == 5,       "not enough money: balance changed")
        && expect(screenHas(screen, "Price: 7"),   "not enough money: price not shown");
}

/// Row 7: the brewer has no coffee - "Sold out", the money stays.
bool testNoIngredientsShowsSoldOut()
{
    std::ostringstream screen;
    VendingMachine vm(screen);
    vm.menu().addDrink(kEspresso);
    // no loadIngredients(): the brewer is empty

    vm.insertCoin(5);
    vm.insertCoin(2);
    vm.selectDrink(0);

    return expect(vm.state() == StateId::HasMoney, "no ingredients: state changed")
        && expect(vm.coins().balance() == 7,       "no ingredients: balance changed")
        && expect(screenHas(screen, "Sold out"),   "no ingredients: 'Sold out' not shown");
}

/// Row 8: the cash is empty, change 3 cannot be paid - "Exact change only".
bool testNoChangeShowsExactChangeOnly()
{
    std::ostringstream screen;
    VendingMachine vm(screen);
    vm.menu().addDrink(kEspresso);
    loadIngredients(vm);
    // no loadCoins(): the cash is empty

    vm.insertCoin(10);
    vm.selectDrink(0);

    return expect(vm.state() == StateId::HasMoney,         "no change: state changed")
        && expect(vm.coins().balance() == 10,              "no change: balance changed")
        && expect(screenHas(screen, "Exact change only"),  "no change: message not shown");
}

/// Rows 9 and 11: exact money - brewing, then straight back to Idle.
/// Also checks that brewing took the ingredients (row 9).
bool testPurchaseWithoutChange()
{
    std::ostringstream screen;
    VendingMachine vm(screen);
    vm.menu().addDrink(kEspresso);
    loadIngredients(vm);

    vm.insertCoin(5);
    vm.insertCoin(2);
    vm.selectDrink(0);

    const bool brewing = expect(vm.state() == StateId::Brewing, "exact money: state is not Brewing")
                      && expect(vm.brewer().amountOf(Ingredient::Coffee) == 92,
                                "exact money: coffee was not taken (100 - 8)");
    if (!brewing)
        return false;

    vm.brewDone();
    return expect(vm.state() == StateId::Idle,          "exact money: not Idle after brewDone")
        && expect(vm.coins().balance() == 0,            "exact money: balance is not 0")
        && expect(screenHas(screen, "Take your drink"), "exact money: 'Take your drink' not shown");
}

/// Rows 9, 10 and 13: 10 for a drink of 7 - the change 3 is paid out.
bool testPurchaseWithChange()
{
    std::ostringstream screen;
    VendingMachine vm(screen);
    vm.menu().addDrink(kEspresso);
    loadIngredients(vm);
    vm.coins().loadCoins(2, 1);
    vm.coins().loadCoins(1, 1);

    vm.insertCoin(10);
    vm.selectDrink(0);
    vm.brewDone();

    const bool paidOut = expect(vm.state() == StateId::Dispensing, "change: state is not Dispensing")
                      && expect(vm.coins().balance() == 0,         "change: balance is not 0")
                      && expect(!vm.coins().canPayOut(3),          "change: coins 2 and 1 still in the cash");
    if (!paidOut)
        return false;

    vm.coinsDispensed();
    return expect(vm.state() == StateId::Idle, "change: not back to Idle after coinsDispensed");
}

/// Row 12 and the entry of OutOfOrder: the money comes back, the machine
/// is out of order and ignores any further coin.
bool testBrewFailureReturnsMoneyAndStops()
{
    std::ostringstream screen;
    VendingMachine vm(screen);
    vm.menu().addDrink(kEspresso);
    loadIngredients(vm);

    vm.insertCoin(5);
    vm.insertCoin(2);
    vm.selectDrink(0);
    vm.brewFailed();

    const bool broken = expect(vm.state() == StateId::OutOfOrder, "failure: state is not OutOfOrder")
                     && expect(vm.coins().balance() == 0,         "failure: money not returned")
                     && expect(screenHas(screen, "Out of order"), "failure: 'Out of order' not shown");
    if (!broken)
        return false;

    vm.insertCoin(5);
    return expect(vm.coins().balance() == 0, "failure: a broken machine accepted a coin");
}

int main()
{
    if (!testInitialStateIsIdle())               return 1;
    if (!testSetStateSwitches())                 return 1;
    if (!testCoinInIdleGoesToHasMoney())         return 1;
    if (!testButtonWithoutMoneyShowsPrice())     return 1;
    if (!testCancelReturnsMoney())               return 1;
    if (!testNotEnoughMoneyShowsPrice())         return 1;
    if (!testNoIngredientsShowsSoldOut())        return 1;
    if (!testNoChangeShowsExactChangeOnly())     return 1;
    if (!testPurchaseWithoutChange())            return 1;
    if (!testPurchaseWithChange())               return 1;
    if (!testBrewFailureReturnsMoneyAndStops())  return 1;

    std::cout << "All Vending Machine tests passed\n";
    return 0;
}
