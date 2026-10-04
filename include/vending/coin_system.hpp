/**
 * @file coin_system.hpp
 * @brief Everything about money: balance, coin stock, change.
 */
#pragma once

#include <map>
#include <optional>

namespace vending {

/**
 * @brief The money part of the vending machine.
 *
 * Responsibilities:
 * - accepts coins and keeps the balance of the current customer;
 * - keeps the stock of coins of every denomination;
 * - answers whether change can be paid;
 * - pays money out.
 *
 * Fake coins are rejected here and never reach the machine.
 *
 * There is no hardware: this class is a model in memory, so tests can
 * use it directly and ask it what happened.
 *
 * Escrow: the customer's coins are kept apart (balance_coins_) until the
 * drink is ready. Cancel or a brewing failure returns exactly those coins.
 * Only commit() moves them into the machine's cash (machine_coins_).
 */
class CoinSystem {

	int reserved_price_ = 0;
	int balance_ = 0;
	std::map<int, int> balance_coins_; // coins in balance

	std::map<int, int> machine_coins_; // coins in machine

	std::map<int, int> last_payout_;   // coins given by the last payOutBalance()

	/// Greedy: which coins from machine_coins_ make up @p amount.
	/// @return coin value -> how many, or std::nullopt if the cash cannot do it.
	std::optional<std::map<int, int>> planPayOut(int amount) const;

public:

	/**
	 * @brief Puts coins into the machine's cash (service: before work starts).
	 * @param value denomination, e.g. 2
	 * @param count how many coins of it
	 */
	void loadCoins(int value, int count);

	/// A valid coin was inserted: it goes to escrow, the balance grows.
	void deposit(int coin);
	/// Remembers the price of the drink being brewed. Coins are not touched.
	void reserve(int price);
	/**
	 * @brief The drink is ready: the purchase becomes final.
	 *
	 * Escrow coins move into the machine's cash, the reserved price is taken
	 * from the balance. What stays on the balance is the change.
	 */
	void commit();
	/**
	 * @brief Pays the whole balance out, the balance becomes 0.
	 *
	 * No commit yet (cancel, failure): returns the escrow coins.
	 * After commit: pays the change from the machine's cash.
	 */
	void payOutBalance();

	/// @return money currently on the balance.
	int balance()const
	{
		return balance_;
	}

	/// @return the change the customer will get: balance minus reserved price.
	int changeDue() const;

	/// @return the coins given by the last payOutBalance(): value -> how many.
	const std::map<int, int>& lastPayout() const
	{
		return last_payout_;
	}

	/// @return true if the machine's cash has coins to pay @p amount out.
	bool canPayOut(int amount) const;
};

} // namespace vending
