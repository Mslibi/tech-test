#ifndef STREAMINGTRADELOADER_H
#define STREAMINGTRADELOADER_H

#include "../Loaders/ITradeLoader.h"
#include "../Models/ITrade.h"
#include "../Models/IScalarResultReceiver.h"
#include "../Models/IPricingEngine.h"
#include "../Models/ITradeReceiver.h"
#include <vector>
#include <map>
#include <string>

class StreamingTradeLoader {
private:
    std::map<std::string, IPricingEngine*> pricers_;

    class PricingReceiver : public ITradeReceiver {
    public:
        StreamingTradeLoader::PricingReceiver::PricingReceiver(std::map<std::string, IPricingEngine*>* pricers, IScalarResultReceiver* resultReceiver)
            : pricers_(pricers), resultReceiver_(resultReceiver) {
        }

        void add(ITrade* trade) override;
    private:
        std::map<std::string, IPricingEngine*>* pricers_;
        IScalarResultReceiver* resultReceiver_;
    };
    
    std::vector<ITradeLoader*> getTradeLoaders();
    void loadPricers();
    
public:
    ~StreamingTradeLoader();
    
    void loadAndPrice(IScalarResultReceiver* resultReceiver);
};

#endif // STREAMINGTRADELOADER_H
