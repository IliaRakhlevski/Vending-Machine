/**
 * @file brewer.cpp
 */
#include "vending/brewer.hpp"

#include <cassert>

namespace vending {

void Brewer::loadIngredient(Ingredient ingredient, int amount)
{
    stock_[ingredient] += amount;
}

bool Brewer::canBrew(const Recipe& recipe) const
{
    for (const auto& [ingredient, needed] : recipe)
    {
        if (amountOf(ingredient) < needed)
            return false;
    }
    return true;
}

void Brewer::brew(const Recipe& recipe)
{
    assert(canBrew(recipe));

    for (const auto& [ingredient, needed] : recipe)
        stock_[ingredient] -= needed;
}

int Brewer::amountOf(Ingredient ingredient) const
{
    // find, not operator[]: operator[] would insert a missing key,
    // and it cannot be used in a const method at all.
    const auto it = stock_.find(ingredient);
    return it == stock_.end() ? 0 : it->second;
}

} // namespace vending
