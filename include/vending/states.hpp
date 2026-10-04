/**
 * @file states.hpp
 * @brief The five concrete states of the vending machine.
 *
 * Each class overrides only the events it reacts to. The numbers in the
 * comments are the rows of the transition table (see README.md).
 */
#pragma once

#include "vending/state.hpp"

namespace vending {

/// Waiting, balance is 0. Rows 1-3.
class IdleState : public State {
public:
    StateId id() const override { return StateId::Idle; }

    void onCoin(VendingMachine& m, int value) override;          // row 1
    void onDrinkSelected(VendingMachine& m, DrinkId drink_id) override; // row 2
    // row 3 (Cancel) does nothing: not overridden
};

/// Waiting, balance is above 0. Rows 4-9.
class HasMoneyState : public State {
public:
    StateId id() const override { return StateId::HasMoney; }

    void onCoin(VendingMachine& m, int value) override;          // row 4
    void onCancel(VendingMachine& m) override;                    // row 5
    void onDrinkSelected(VendingMachine& m, DrinkId drink_id) override; // rows 6-9
};

/// The brewer is preparing a drink. Rows 10-12.
class BrewingState : public State {
public:
    StateId id() const override { return StateId::Brewing; }

    void onBrewDone(VendingMachine& m) override;   // rows 10-11
    void onBrewFailed(VendingMachine& m) override; // row 12
};

/// The coin system is paying the balance out. Row 13.
class DispensingState : public State {
public:
    StateId id() const override { return StateId::Dispensing; }

    void onEnter(VendingMachine& m) override;          // entry: pay the whole balance out
    void onCoinsDispensed(VendingMachine& m) override; // row 13
};

/// Broken. No way out until service mode exists.
class OutOfOrderState : public State {
public:
    StateId id() const override { return StateId::OutOfOrder; }

    void onEnter(VendingMachine& m) override; // entry: pay the balance out, show "Out of order"
};

} // namespace vending
