/**
 * @file menu.hpp
 * @brief The drinks the machine sells: names and prices.
 */
#pragma once

#include "vending/recipe.hpp"
#include "vending/state.hpp"   // DrinkId

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace vending {

/**
 * @brief One drink of the menu.
 *
 * The recipe lives inside the drink, so a drink and its recipe can never
 * get out of sync.
 */
struct Drink {
    std::string name;   ///< shown to the user, e.g. "Espresso"
    int price = 0;      ///< in whole shekels, an int like the balance
    Recipe recipe{};    ///< what the Brewer needs to make it (may be left out)
};

/**
 * @brief All drinks the machine sells.
 *
 * A drink's DrinkId is its position in the menu: the first added drink is
 * 0, the second is 1, and so on. The id is not stored inside Drink.
 *
 * Converting the label on a button ("1", "2"...) to a DrinkId is the job
 * of the UI, not of the menu.
 */
class Menu {
public:
    /**
     * @brief Adds a drink at the end of the menu.
     * @return the DrinkId of the added drink.
     */
    DrinkId addDrink(const Drink& drink)
    {
        drinks_.push_back(drink);
        return drinks_.size() - 1;
    }

    /**
     * @brief The drink with the given id.
     * @throws std::out_of_range if @p id is not in the menu.
     */
    const Drink& drink(DrinkId id) const
    {
        return drinks_.at(id);
    }

    /// @return how many drinks are in the menu.
    std::size_t size() const
    {
        return drinks_.size();
    }

    /// @return all drinks, read-only. Index in the vector = DrinkId.
    const std::vector<Drink>& drinks() const
    {
        return drinks_;
    }

private:
    std::vector<Drink> drinks_;
};

} // namespace vending
