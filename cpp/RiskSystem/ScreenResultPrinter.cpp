#include "ScreenResultPrinter.h"
#include <iostream>

void ScreenResultPrinter::printResults(ScalarResults& results) {
    for (const auto& result : results) {

        std::cout << result.getTradeId();

        // If there is a result
        if (result.getResult().has_value()) {
            std::cout << " : " << result.getResult().value();
        }
        // If there is an Error
        if (result.getError().has_value()) {
            if (!result.getError().value().empty()) {
                std::cout << " : " << result.getError().value() << std::endl;
            }
            else {
                std::cout << std::endl;
            }
        }
        else {
            std::cout << std::endl;
        }
        // Write code here to print out the results such that we have:
        // TradeID : Result : Error
        // If there is no result then the output should be:
        // TradeID : Error
        // If there is no error the output should be:
        // TradeID : Result
    }
}
