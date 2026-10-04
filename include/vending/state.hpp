/**
 * @file state.hpp
 * @brief Base class of all vending machine states (the State pattern).
 */
#pragma once

#include <cstddef>

namespace vending {

class VendingMachine;

/// Index of a drink in the menu.
using DrinkId = std::size_t;

/**
 * @brief Identifies a state without looking at its class.
 *
 * Used by tests and by the UI to ask "where is the machine now?".
 */
enum class StateId {
    Idle,        ///< waiting, balance is 0
    HasMoney,    ///< waiting, balance is above 0
    Brewing,     ///< the brewer is preparing a drink
    Dispensing,  ///< the coin system is paying money out
    OutOfOrder   ///< broken, waits for a technician
};

/**
 * @brief One state of the machine.
 *
 * There is one virtual method per event. Every method does nothing by
 * default, so a derived state overrides only the events it reacts to.
 * An event that is not overridden is ignored.
 *
 * Every override in a derived class must be marked `override`.
 */
class State {
public:
    virtual ~State() = default;

    /// @return which state this is.
    virtual StateId id() const = 0;

    /// Called when the machine enters this state.
    virtual void onEnter(VendingMachine&) {}
    /// Called when the machine leaves this state.
    virtual void onExit(VendingMachine&) {}

    /// A valid coin was inserted. Fake coins never reach the machine.
    virtual void onCoin(VendingMachine&, int /*value*/) {}
    /// A drink button was pressed.
    virtual void onDrinkSelected(VendingMachine&, DrinkId) {}
    /// The Cancel button was pressed.
    virtual void onCancel(VendingMachine&) {}
    /// The brewer reports that the drink is ready.
    virtual void onBrewDone(VendingMachine&) {}
    /// The brewer reports a failure.
    virtual void onBrewFailed(VendingMachine&) {}
    /// The coin system has finished paying money out.
    virtual void onCoinsDispensed(VendingMachine&) {}
};

} // namespace vending
