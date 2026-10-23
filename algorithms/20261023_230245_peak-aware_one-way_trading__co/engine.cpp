#include <vector>

#include "types.hpp"

namespace peaktrading {

TradeResult computeMaxProfit(const std::vector<int>& prices) {
    if (prices.size() < 2) {
        return TradeResult(-1, -1, 0);
    }
    int minPrice = prices[0];
    int minIndex = 0;
    int maxProfit = 0;
    int buyIndex = -1;
    int sellIndex = -1;

    for (size_t i = 1; i < prices.size(); ++i) {
        int price = prices[i];
        int profit = price - minPrice;
        if (profit > maxProfit) {
            maxProfit = profit;
            buyIndex = minIndex;
            sellIndex = static_cast<int>(i);
        }
        if (price < minPrice) {
            minPrice = price;
            minIndex = static_cast<int>(i);
        }
    }

    if (maxProfit <= 0) {
        return TradeResult(-1, -1, 0);
    }
    return TradeResult(buyIndex, sellIndex, maxProfit);
}

TradeResult computePeakAwareProfit(const std::vector<int>& prices, int predictedPeak) {
    if (prices.empty()) {
        return TradeResult(-1, -1, 0);
    }

    int sellIndex = -1;
    for (size_t i = 0; i < prices.size(); ++i) {
        if (prices[i] >= predictedPeak) {
            sellIndex = static_cast<int>(i);
            break;
        }
    }

    if (sellIndex <= 0) {
        return TradeResult(-1, -1, 0);
    }

    int minPrice = prices[0];
    int buyIndex = 0;
    for (int i = 1; i < sellIndex; ++i) {
        if (prices[i] < minPrice) {
            minPrice = prices[i];
            buyIndex = i;
        }
    }

    int profit = prices[sellIndex] - minPrice;
    if (profit <= 0) {
        return TradeResult(-1, -1, 0);
    }
    return TradeResult(buyIndex, sellIndex, profit);
}

} // namespace peaktrading
