#include "StreamingTradeLoader.h"
#include "../Loaders/BondTradeLoader.h"
#include "../Loaders/FxTradeLoader.h"
#include "PricingConfigLoader.h"
#include "../Pricers/GovBondPricingEngine.h"
#include "../Pricers/CorpBondPricingEngine.h"
#include "../Pricers/FxPricingEngine.h"
#include <stdexcept>

std::vector<ITradeLoader*> StreamingTradeLoader::getTradeLoaders() {
    std::vector<ITradeLoader*> loaders;
    
    BondTradeLoader* bondLoader = new BondTradeLoader();
    bondLoader->setDataFile("TradeData/BondTrades.dat");
    loaders.push_back(bondLoader);
    
    FxTradeLoader* fxLoader = new FxTradeLoader();
    fxLoader->setDataFile("TradeData/FxTrades.dat");
    loaders.push_back(fxLoader);
    
    return loaders;
}

void StreamingTradeLoader::loadPricers() {
    PricingConfigLoader pricingConfigLoader;
    pricingConfigLoader.setConfigFile("./PricingConfig/PricingEngines.xml");
    PricingEngineConfig pricerConfig = pricingConfigLoader.loadConfig();

    for (const auto& configItem : pricerConfig) {
        if (configItem.getTypeName().find("GovBondPricingEngine") != std::string::npos) {
            IPricingEngine* engine = new GovBondPricingEngine();
            pricers_["GovBond"] = engine;
        }
        else if (configItem.getTypeName().find("CorpBondPricingEngine") != std::string::npos) {
            IPricingEngine* engine = new CorpBondPricingEngine();
            pricers_["CorpBond"] = engine;
        }
        else if (configItem.getTypeName().find("FxPricingEngine") != std::string::npos) {
            IPricingEngine* engine = new FxPricingEngine();

            if (configItem.getTradeType().find("FxSpot") != std::string::npos) {
                pricers_["FxSpot"] = engine;
            }
            else if (configItem.getTradeType().find("FxFwd") != std::string::npos) {
                pricers_["FxFwd"] = engine;
            }
            else {
                delete engine; // constructed an FxPricingEngine but couldn't place it anywhere meaningful
            }
        }
    }
}

StreamingTradeLoader::~StreamingTradeLoader() {
    for (auto& pair : pricers_) {
        delete pair.second;
    }
}

void StreamingTradeLoader::loadAndPrice(IScalarResultReceiver* resultReceiver) {
    loadPricers();

    PricingReceiver receiver(&pricers_, resultReceiver);
    
    for (auto const loader : getTradeLoaders()){
        loader->streamTrades(receiver);

        delete loader;
    }
}

void StreamingTradeLoader::PricingReceiver::add(ITrade* trade) {
    auto pricerIt = pricers_->find(trade->getTradeType());
    if (pricerIt != pricers_->end())
    {
        IPricingEngine* engine = pricerIt->second;
        engine->price(trade, resultReceiver_);
    }
    else {
        resultReceiver_->addError(trade->getTradeId(), "No Pricing Engines available for this trade type");
    }
}
