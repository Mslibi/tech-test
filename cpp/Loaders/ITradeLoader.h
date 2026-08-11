#ifndef ITRADELOADER_H
#define ITRADELOADER_H

#include "../Models/ITrade.h"
#include "../Models/ITradeReceiver.h"
#include <vector>
#include <string>

class ITradeLoader {
public:
    virtual ~ITradeLoader() = default;
    virtual std::vector<ITrade*> loadTrades() = 0;
    virtual std::string getDataFile() const = 0;
    virtual void setDataFile(const std::string& file) = 0;

    virtual void streamTrades(ITradeReceiver& receiver) = 0;
};

#endif // ITRADELOADER_H
