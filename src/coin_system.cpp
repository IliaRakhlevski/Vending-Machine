/**
 * @file coin_system.cpp
 * @brief Money: escrow, cash, change. The contract of every method is in
 *        coin_system.hpp.
 */
#include "vending/coin_system.hpp"
#include <algorithm>
#include <cassert>

namespace vending {

	void CoinSystem::loadCoins(int value, int count)
	{
		machine_coins_[value] += count;
	}

	void CoinSystem::deposit(int coin)
	{
		balance_ += coin;
		balance_coins_[coin] += 1;
	}

	void CoinSystem::reserve(int price)
	{
		reserved_price_ = price;
	}

	void CoinSystem::commit()
	{
		for (const auto& [value, count] : balance_coins_)
			machine_coins_[value] += count;

		balance_coins_.clear();

		balance_ -= reserved_price_;   // what stays is the change
		reserved_price_ = 0;           // no purchase in progress any more
	}

	int CoinSystem::changeDue() const
	{
		return (balance_ - reserved_price_);
	}

	void CoinSystem::payOutBalance()
	{
		if (!balance_coins_.empty())
		{
			last_payout_ = balance_coins_;   // the customer's own coins go back
			balance_coins_.clear();
			balance_ = 0;
			reserved_price_ = 0;
		}
		else
		{
			std::optional<std::map<int, int>> plan = planPayOut(balance_);
			assert(plan.has_value());

			for (const auto& [value, count] : plan.value())
				machine_coins_[value] -= count;

			last_payout_ = plan.value();
		
			balance_ = 0;
		}
	}

	bool CoinSystem::canPayOut(int amount) const
	{
		return planPayOut(amount).has_value();
	}

	std::optional<std::map<int, int>> CoinSystem::planPayOut(int amount) const
	{
		std::map<int, int> coins;
		int rest = amount;

		for (auto it = machine_coins_.rbegin(); it != machine_coins_.rend(); ++it)
		{
			const auto& [value, count] = *it;
			int num_coins = std::min(rest / value, count);

			if (num_coins == 0)
				continue;

			coins[value] = num_coins;
			rest -= num_coins * value;
			if (rest == 0)
				break;
		}

		if (rest == 0)
			return coins;

		return std::nullopt;
	}

} // namespace vending
