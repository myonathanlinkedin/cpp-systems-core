#pragma once
#include <vector>
#include <algorithm>
#include <cassert>

namespace peaktrading {

struct TradeResult {
    int buyIndex;
    int sellIndex;
    int profit;
    TradeResult() = default;
    TradeResult(int b, int s, int p) : buyIndex(b), sellIndex(s), profit(p) {};
};

// Classic maximum profit: buy once, sell once later
TradeResult computeMaxProfit(const std::vector<int>& prices);

// Peak-aware algorithm: given predicted peak price, buy at min before peak, sell at peak if profitable
TradeResult computePeakAwareProfit(const std::vector<int>& prices, int predictedPeak);

} // namespace peaktrading
