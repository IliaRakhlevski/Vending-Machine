/**
 * @file recipe.hpp
 * @brief Ingredients and recipes. Shared by the Menu (Drink) and the Brewer.
 */
#pragma once

#include <map>

namespace vending {

/**
 * @brief Everything a drink can be made of.
 *
 * An enum class, not a string: a typo in a name is a compile error, not a
 * silent bug. Amounts are in grams or millilitres, depending on the
 * ingredient.
 */
enum class Ingredient {
    Coffee,   ///< grams
    Water,    ///< millilitres
    Milk,     ///< millilitres
    Sugar     ///< grams
};

/// What one drink needs: ingredient -> amount.
using Recipe = std::map<Ingredient, int>;

} // namespace vending
