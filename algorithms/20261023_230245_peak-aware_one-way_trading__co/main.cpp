#include <iostream>
#include <vector>
#include <cassert>
#include "types.hpp"

using namespace peaktrading;

void testComputeMaxProfit() {
    {
        std::vector<int> prices;
        TradeResult res = computeMaxProfit(prices);
        assert(res.profit == 0 && res.buyIndex == -1 && res.sellIndex == -1);
    }
    {
        std::vector<int> prices = {5};
        TradeResult res = computeMaxProfit(prices);
        assert(res.profit == 0 && res.buyIndex == -1 && res.sellIndex == -1);
    }
    {
        std::vector<int> prices = {5, 4, 3, 2, 1};
        TradeResult res = computeMaxProfit(prices);
        assert(res.profit == 0 && res.buyIndex == -1 && res.sellIndex == -1);
    }
    {
        std::vector<int> prices = {1, 2, 3, 4, 5};
        TradeResult res = computeMaxProfit(prices);
        assert(res.profit == 4 && res.buyIndex == 0 && res.sellIndex == 4);
    }
    {
        std::vector<int> prices = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
        TradeResult res = computeMaxProfit(prices);
        assert(res.profit == 8 && res.buyIndex == 1 && res.sellIndex == 5);
    }
}

void testComputePeakAwareProfit() {
    std::vector<int> prices = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};

    {
        int predictedPeak = 9;
        TradeResult res = computePeakAwareProfit(prices, predictedPeak);
        assert(res.profit == 8 && res.buyIndex == 1 && res.sellIndex == 5);
    }
    {
        int predictedPeak = 6;
        TradeResult res = computePeakAwareProfit(prices, predictedPeak);
        assert(res.profit == 5 && res.buyIndex == 1 && res.sellIndex == 7);
    }
    {
        int predictedPeak = 10;
        TradeResult res = computePeakAwareProfit(prices, predictedPeak);
        assert(res.profit == 0 && res.buyIndex == -1 && res.sellIndex == -1);
    }
    {
        int predictedPeak = 2;
        TradeResult res = computePeakAwareProfit(prices, predictedPeak);
        assert(res.profit == 0 && res.buyIndex == -1 && res.sellIndex == -1);
    }
    {
        std::vector<int> single = {5};
        TradeResult res = computePeakAwareProfit(single, 5);
        assert(res.profit == 0 && res.buyIndex == -1 && res.sellIndex == -1);
    }
}

int main() {
    testComputeMaxProfit();
    testComputePeakAwareProfit();
    std::cout << "All tests passed successfully." << std::endl;
    return 0;
}
