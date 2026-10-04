/**
 * @file ui.hpp
 * @brief What the machine shows to the user. Output only.
 */
#pragma once

#include <ostream>

namespace vending {

class Menu;

/**
 * @brief The display of the vending machine.
 *
 * Output only: the UI shows things, it decides nothing and stores no
 * prices or balances. It gets the data from the caller.
 *
 * The text goes to a stream given in the constructor: std::cout in the
 * program, a std::ostringstream in tests (then the test reads what was
 * "shown" as a string).
 *
 * Reading the user's input (coins, buttons) is not here: that is the job
 * of the console front end in app/, which calls the machine's events.
 */
class UI {
public:
    /// @param out where the text goes. Must outlive the UI.
    explicit UI(std::ostream& out) : out_(out) {}

    /// Lists the drinks, numbered for humans from 1 (button 1 = DrinkId 0).
    void showMenu(const Menu& menu);

    /// "Balance: N"
    void showBalance(int balance);

    /// Not enough money (rows 2 and 6): "Price: N"
    void showPrice(int price);

    /// No ingredients for the drink (row 7).
    void showSoldOut();

    /// Change cannot be paid (row 8).
    void showExactChangeOnly();

    /// The drink is ready (rows 10-11).
    void showTakeDrink();

    /// The machine is broken (entry of OutOfOrder).
    void showOutOfOrder();

private:
    std::ostream& out_;
};

} // namespace vending
