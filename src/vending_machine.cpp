/**
 * @file vending_machine.cpp
 * @brief Event forwarding and the state switch.
 *
 * Every external event goes to the current state. setState() is the only
 * place where the current state changes.
 */
#include "vending/vending_machine.hpp"

namespace vending {

void VendingMachine::insertCoin(int value) { current_->onCoin(*this, value); }
void VendingMachine::selectDrink(DrinkId id) { current_->onDrinkSelected(*this, id); }
void VendingMachine::cancel() { current_->onCancel(*this); }
void VendingMachine::brewDone() { current_->onBrewDone(*this); }
void VendingMachine::brewFailed() { current_->onBrewFailed(*this); }
void VendingMachine::coinsDispensed() { current_->onCoinsDispensed(*this); }

void VendingMachine::setState(State& next)
{
    current_->onExit(*this);
    current_ = &next;
    current_->onEnter(*this);
}

} // namespace vending
