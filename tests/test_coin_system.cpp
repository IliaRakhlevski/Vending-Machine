/**
 * @file test_coin_system.cpp
 * @brief Tests of CoinSystem. Returns 0 when every check passes.
 */
#include "vending/coin_system.hpp"

#include <iostream>

using namespace vending;


/// Inserted 10, drink costs 7: balance is still 10, change will be 3.
bool testReserveKeepsBalanceAndComputesChange()
{
    CoinSystem coins;

    coins.deposit(10);
    coins.reserve(7);

    if (coins.balance() == 10 && coins.changeDue() == 3)
        return true;

    std::cout << "FAILED: Balance or Change Due are incorrect" << std::endl;

    return false;
}

/// Inserted 10, drink costs 7, drink is ready: balance becomes 3.
bool testCommitLeavesChangeOnBalance()
{
    CoinSystem coins;

    coins.deposit(10);
    coins.reserve(7);
    coins.commit();

    if (coins.balance() == 3)
        return true;

    std::cout << "FAILED: Balance is incorrect" << std::endl;

    return false;
}

/// Inserted exactly 5 for a drink of 5: nothing left after commit.
bool testCommitExactAmountLeavesZero()
{
    CoinSystem coins;

    coins.deposit(5);
    coins.reserve(5);
    coins.commit();

    if (coins.balance() == 0)
        return true;

    std::cout << "FAILED: Balance must be 0" << std::endl;

    return false;
}


/// Cash has 2+2+2+1: 3 can be paid out, 8 cannot.
bool testCanPayOutChecksTheCash()
{
    CoinSystem coins;

    coins.loadCoins(2, 3); 
    coins.loadCoins(1, 1);

    if (coins.canPayOut(3) == true && coins.canPayOut(8) == false)
        return true;

    std::cout << "FAILED: Cash has 2+2+2+1: 3 can be paid out, 8 cannot" << std::endl;

    return false;
}

/// Cash 2+1, customer pays 10 for 7: change 3 is paid out from the cash.
bool testPayOutBalancePaysChangeFromCash()
{
    CoinSystem coins;

    coins.loadCoins(2, 1);
    coins.loadCoins(1, 1);

    coins.deposit(10);
    coins.reserve(7);
    coins.commit();

    coins.payOutBalance();

    if (coins.balance() == 0 && coins.canPayOut(3) == false)
        return true;

    std::cout << "FAILED: Cash 2+1, customer pays 10 for 7: change 3 is paid out from the cash" << std::endl;

    return false;
}

/// Customer inserted 5+2 and cancelled: own coins back, balance 0.
bool testPayOutBalanceReturnsEscrowOnCancel()
{
    CoinSystem coins;

    coins.deposit(5);
    coins.deposit(2);

    coins.payOutBalance();

    if (coins.balance() == 0)
        return true;

    std::cout << "FAILED: Customer inserted 5+2 and cancelled: own coins back, balance 0" << std::endl;

    return false;
}


int main()
{
    if (!testReserveKeepsBalanceAndComputesChange()) return 1;
    if (!testCommitLeavesChangeOnBalance())          return 1;
    if (!testCommitExactAmountLeavesZero())          return 1;
    if (!testCanPayOutChecksTheCash())               return 1;
    if (!testPayOutBalancePaysChangeFromCash())      return 1;
    if (!testPayOutBalanceReturnsEscrowOnCancel())   return 1;

   
    std::cout << "All Coin System tests passed\n";
    return 0;
}
