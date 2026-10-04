/**
 * @file brewer.hpp
 * @brief Prepares drinks and keeps the stock of ingredients.
 */
#pragma once

#include "vending/recipe.hpp"

#include <map>

namespace vending {

/**
 * @brief The brewing part of the vending machine.
 *
 * Knows only recipes, not drinks: names and prices are not its business.
 *
 * There is no hardware. brew() only takes the ingredients from the stock.
 * In a real machine brewing lasts 20-30 seconds and ends with the
 * machine's brewDone() or brewFailed() event; here the test or main()
 * calls those events instead.
 */
class Brewer {
public:
    /**
     * @brief Puts an ingredient into the machine (service: before work starts).
     * @param ingredient what is loaded
     * @param amount     how much, in grams or millilitres
     */
    void loadIngredient(Ingredient ingredient, int amount);

    /// @return true if the stock has enough of every ingredient of @p recipe.
    bool canBrew(const Recipe& recipe) const;

    /**
     * @brief Starts brewing: takes the ingredients of @p recipe from the stock.
     * @pre canBrew(recipe) - checked by the state before (row 7).
     */
    void brew(const Recipe& recipe);

    /// @return how much of @p ingredient is left (0 if it was never loaded).
    int amountOf(Ingredient ingredient) const;

private:
    std::map<Ingredient, int> stock_;   ///< ingredient -> amount left
};

} // namespace vending
