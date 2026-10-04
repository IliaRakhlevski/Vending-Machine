/**
 * @file ui.cpp
 */
#include "vending/ui.hpp"
#include "vending/menu.hpp"

namespace vending {

void UI::showMenu(const Menu& menu)
{
    const auto& drinks = menu.drinks();
    for (std::size_t i = 0; i < drinks.size(); ++i)
        out_ << (i + 1) << ". " << drinks[i].name << " - " << drinks[i].price << "\n";
}

void UI::showBalance(int balance)
{
    out_ << "Balance: " << balance << "\n";
}

void UI::showPrice(int price)
{
    out_ << "Price: " << price << "\n";
}

void UI::showSoldOut()
{
    out_ << "Sold out\n";
}

void UI::showExactChangeOnly()
{
    out_ << "Exact change only\n";
}

void UI::showTakeDrink()
{
    out_ << "Take your drink\n";
}

void UI::showOutOfOrder()
{
    out_ << "Out of order\n";
}

} // namespace vending
