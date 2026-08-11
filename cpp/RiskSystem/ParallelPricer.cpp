#include "ParallelPricer.h"
#include "../Pricers/GovBondPricingEngine.h"
#include "../Pricers/CorpBondPricingEngine.h"
#include "../Pricers/FxPricingEngine.h"
#include <stdexcept>

ParallelPricer::~ParallelPricer() {
    for (auto& pair : pricers_) {
        delete pair.second;
    }
}

void ParallelPricer::loadPricers() {
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
                delete engine;
            }
        }
    }
}

void ParallelPricer::price(const std::vector<std::vector<ITrade*>>& tradeContainers, 
                           IScalarResultReceiver* resultReceiver) {
    loadPricers();

    ThreadSafeReceiver safeReceiver(resultReceiver, &resultMutex_);
    std::vector<std::thread> threads;

    for (const auto& tradeContainer : tradeContainers) {
        for (ITrade* trade : tradeContainer) {
            threads.emplace_back([this, trade, &safeReceiver]() {
                auto it = pricers_.find(trade->getTradeType());
                if (it == pricers_.end()) {
                    safeReceiver.addError(trade->getTradeId(), "No Pricing Engines available for this trade type");
                    return;
                }
                it->second->price(trade, &safeReceiver);
                });
        }
    }

    for (auto& t : threads) {
        t.join();
    }
}

