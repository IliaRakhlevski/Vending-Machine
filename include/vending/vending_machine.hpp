/**
 * @file vending_machine.hpp
 * @brief The Context of the State pattern: owns the states and receives events.
 */
#pragma once

#include "vending/brewer.hpp"
#include "vending/coin_system.hpp"
#include "vending/menu.hpp"
#include "vending/state.hpp"
#include "vending/states.hpp"
#include "vending/ui.hpp"

#include <iostream>

namespace vending {

/**
 * @brief The coffee vending machine.
 *
 * Receives every external event and forwards it to the current state.
 * The five states are ordinary members: no heap, they live exactly as
 * long as the machine.
 *
 * @invariant current_ points to one of the five state members of *this.
 */
class VendingMachine {
public:
    /**
     * @param out where the UI writes its text: std::cout by default,
     *            a std::ostringstream in tests that check the messages.
     */
    explicit VendingMachine(std::ostream& out = std::cout) : ui_(out) {}

    // current_ points into *this. A copy would point into the original.
    VendingMachine(const VendingMachine&) = delete;
    VendingMachine& operator=(const VendingMachine&) = delete;
    VendingMachine(VendingMachine&&) = delete;
    VendingMachine& operator=(VendingMachine&&) = delete;

    /// @name External events
    /// Each one is forwarded to the current state.
    /// @{
    void insertCoin(int value);
    void selectDrink(DrinkId id);
    void cancel();
    void brewDone();
    void brewFailed();
    void coinsDispensed();
    /// @}

    /// @return the state the machine is in now.
    StateId state() const { return current_->id(); }

    /**
     * @brief Leaves the current state and enters @p next.
     *
     * Order: onExit of the old state, switch, onEnter of the new state.
     * Called by the states themselves.
     */
    void setState(State& next);

    /// @name The states, for setState()
    /// @{
    State& idle() { return idle_; }
    State& hasMoney() { return hasMoney_; }
    State& brewing() { return brewing_; }
    State& dispensing() { return dispensing_; }
    State& outOfOrder() { return outOfOrder_; }
    /// @}

    /// @name The subsystems, for the states
    /// @{
    CoinSystem& coins() { return coins_; }
    Brewer& brewer() { return brewer_; }
    Menu& menu() { return menu_; }
    UI& ui() { return ui_; }
    /// @}

private:
    CoinSystem coins_;
    Brewer brewer_;
    Menu menu_;   ///< filled after construction: machine.menu().addDrink(...)
    UI ui_;

    IdleState idle_;
    HasMoneyState hasMoney_;
    BrewingState brewing_;
    DispensingState dispensing_;
    OutOfOrderState outOfOrder_;

    State* current_ = &idle_;
};

} // namespace vending
