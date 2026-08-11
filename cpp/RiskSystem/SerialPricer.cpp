#include "SerialPricer.h"
#include <stdexcept>
#include "../Pricers/GovBondPricingEngine.h"
#include "../Pricers/CorpBondPricingEngine.h"
#include "../Pricers/FxPricingEngine.h"

SerialPricer::~SerialPricer() {
    for (auto& pair : pricers_) {
        delete pair.second;
    }
}

void SerialPricer::loadPricers() {
    PricingConfigLoader pricingConfigLoader;
    pricingConfigLoader.setConfigFile("./PricingConfig/PricingEngines.xml");
    PricingEngineConfig pricerConfig = pricingConfigLoader.loadConfig();
    
    for (const auto& configItem : pricerConfig) {        
        if (configItem.getTypeName().find("GovBondPricingEngine") != std::string::npos){
            IPricingEngine* engine = new GovBondPricingEngine();
            pricers_["GovBond"] = engine;
        }
        else if(configItem.getTypeName().find("CorpBondPricingEngine") != std::string::npos){
            IPricingEngine* engine = new CorpBondPricingEngine();
            pricers_["CorpBond"] = engine;
        }
        else if (configItem.getTypeName().find("FxPricingEngine") != std::string::npos){
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

void SerialPricer::price(const std::vector<std::vector<ITrade*>>& tradeContainers, 
                         IScalarResultReceiver* resultReceiver) {
    loadPricers();
    
    for (const auto& tradeContainer : tradeContainers) {
        for (ITrade* trade : tradeContainer) {
            std::string tradeType = trade->getTradeType();
            if (pricers_.find(tradeType) == pricers_.end()) {
                resultReceiver->addError(trade->getTradeId(), "No Pricing Engines available for this trade type");
                continue;
            }
            
            IPricingEngine* pricer = pricers_[tradeType];
            pricer->price(trade, resultReceiver);
        }
    }
}
