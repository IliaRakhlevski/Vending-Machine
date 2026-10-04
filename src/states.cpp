/**
 * @file states.cpp
 * @brief Reactions of the states. Row numbers refer to the transition table.
 *
 * Every method is one reaction from the transition table in README.md:
 * it checks the conditions, runs the transition actions, and only then
 * calls setState(). Entry actions (onEnter) run inside setState().
 *
 * An event a state does not override goes to the empty method of State
 * and is ignored.
 */

#include <cassert>
#include "vending/states.hpp"
#include "vending/vending_machine.hpp"

namespace vending {

// ---------------------------------------------------------------- Idle

/// Row 1: the first coin goes to the balance, the machine now has money.
void IdleState::onCoin(VendingMachine& m, int value)
{
    m.coins().deposit(value);
    m.setState(m.hasMoney());
}

/// Row 2: a button pressed with no money - only show the price.
void IdleState::onDrinkSelected(VendingMachine& m, DrinkId drink_id)
{
    const Drink& drink = m.menu().drink(drink_id);
    m.ui().showPrice(drink.price);
}

// Row 3 (Cancel in Idle) does nothing. IdleState does not override onCancel,
// so the empty State::onCancel runs and the event is ignored.

// ---------------------------------------------------------------- HasMoney

/// Row 4: one more coin. The state does not change, so no setState().
void HasMoneyState::onCoin(VendingMachine& m, int value)
{
    m.coins().deposit(value);
}

/// Row 5: cancel. Paying the money back is the entry action of Dispensing.
void HasMoneyState::onCancel(VendingMachine& m)
{
    m.setState(m.dispensing());
}

/**
 * Rows 6-9: a button pressed with money on the balance.
 *
 * The checks go in the order of the table. A failed check shows a message
 * and returns: the machine stays in HasMoney. Only when every check passes
 * the purchase starts (row 9).
 */
void HasMoneyState::onDrinkSelected(VendingMachine& m, DrinkId drink_id)
{
    const Drink& drink = m.menu().drink(drink_id);

    // row 6: not enough money -> show the price, stay
    if (m.coins().balance() < drink.price)
    {
        m.ui().showPrice(drink.price);
        return;
    }
    // row 7: no ingredients -> "Sold out", stay
    if (!m.brewer().canBrew(drink.recipe))
    {
        m.ui().showSoldOut();
        return;
    }
    // row 8: change cannot be paid -> "Exact change only", stay
    int change = m.coins().balance() - drink.price;
    if (!m.coins().canPayOut(change))
    {
        m.ui().showExactChangeOnly();
        return;
    }
    // row 9: everything is fine -> reserve the price, start the brewer, go
    m.coins().reserve(drink.price);
    m.brewer().brew(drink.recipe);
    m.setState(m.brewing());
}

// ---------------------------------------------------------------- Brewing

/**
 * Rows 10-11: the drink is ready.
 *
 * The change is read before commit(): conditions choose the row, actions
 * run after. It cannot be negative - row 6 did not let us get here with
 * too little money - so a negative value is a bug, caught by assert.
 * "Take your drink" is a transition action, so it comes before setState().
 */
void BrewingState::onBrewDone(VendingMachine& m)
{
    int change = m.coins().changeDue();
    assert(change >= 0);

    m.coins().commit();

    m.ui().showTakeDrink();

    if (change > 0) 
        m.setState(m.dispensing());
    else    
        m.setState(m.idle());
}

/// Row 12: brewing failed. No refund here: there was no commit(), the
/// customer's coins are still in escrow, the entry of OutOfOrder returns them.
void BrewingState::onBrewFailed(VendingMachine& m)
{
    m.setState(m.outOfOrder());
}

// ---------------------------------------------------------------- Dispensing

/// Entry action: pay the balance out - escrow coins after a cancel, or the
/// change from the cash after a purchase. Rows 5 and 10 both lead here.
void DispensingState::onEnter(VendingMachine& m)
{
    m.coins().payOutBalance();
}

/// Row 13: the coins are out, back to waiting.
void DispensingState::onCoinsDispensed(VendingMachine& m)
{
    m.setState(m.idle());
}

// ---------------------------------------------------------------- OutOfOrder

/// Entry action: show "Out of order" and return the customer's money.
/// There is no way out of this state until service mode exists.
void OutOfOrderState::onEnter(VendingMachine& m)
{
    m.ui().showOutOfOrder();
    m.coins().payOutBalance();
}

} // namespace vending
