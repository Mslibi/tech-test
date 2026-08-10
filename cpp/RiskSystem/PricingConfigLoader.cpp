#include "PricingConfigLoader.h"
#include <stdexcept>
#include "pugixml.hpp"

std::string PricingConfigLoader::getConfigFile() const {
    return configFile_;
}

void PricingConfigLoader::setConfigFile(const std::string& file) {
    configFile_ = file;
}

PricingEngineConfig PricingConfigLoader::loadConfig() {
    return parseXml(configFile_);
}

PricingEngineConfig PricingConfigLoader::parseXml(const std::string& filename)
{
    PricingEngineConfig config;
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename.c_str());

    if (!result) {
        throw std::runtime_error("Failed to parse XML: " + std::string(result.description()));
    }

    for (pugi::xml_node engine : doc.child("PricingEngines").children("Engine")) {
        PricingEngineConfigItem item;

        item.setTradeType(engine.attribute("tradeType").value());
        item.setAssembly(engine.attribute("assembly").value());
        item.setTypeName(engine.attribute("pricingEngine").value());
        config.push_back(item);
    }

    return config;
}

