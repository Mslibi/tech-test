#ifndef FXTRADELOADER_H
#define FXTRADELOADER_H

#include "ITradeLoader.h"
#include "../Models/FxTrade.h"
#include "../Models/TradeList.h"
#include "../Models/ITradeReceiver.h"
#include <string>
#include <vector>

class FxTradeLoader : public ITradeLoader {
private:
    static inline const std::string separator = "\xC2\xAC"; // UTF-8 encoding of the NOT SIGN character (U+00AC)
    std::string dataFile_;

    FxTrade* createTradeFromLine(const std::string& line);
    void loadTradesFromFile(const std::string& filename, ITradeReceiver& receiver);
    
    void splitOnSeparator(const std::string& line, const std::string& delimiter, std::vector<std::string>& items);
public:
    // NOTE: These methods are only here to allow the solution to compile prior to the test being completed.
    std::vector<ITrade*> loadTrades() override;
    std::string getDataFile() const override;
    void setDataFile(const std::string& file) override;

    void streamTrades(ITradeReceiver& receiver) override;
};

#endif // FXTRADELOADER_H
