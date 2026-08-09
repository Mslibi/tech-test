#include "FxTradeLoader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <chrono>
#include <string>
#include <vector>

void FxTradeLoader::splitOnSeparator(const std::string& line, const std::string& delimiter, std::vector<std::string>& items) {
    size_t start = 0, pos;
    while ((pos = line.find(delimiter, start)) != std::string::npos) {
        items.push_back(line.substr(start, pos - start));
        start = pos + delimiter.size();
    }
    items.push_back(line.substr(start));
}

FxTrade* FxTradeLoader::createTradeFromLine(const std::string& line) {
    std::vector<std::string> items;

    splitOnSeparator(line, separator, items);
    
    if (items.size() < 9) {
        throw std::runtime_error("Invalid line format");
    }

    FxTrade* trade = new FxTrade(items[8], items[0]);

    std::tm tm = {};
    std::istringstream dateStream(items[1]);
    dateStream >> std::get_time(&tm, "%Y-%m-%d");
    auto timePoint = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    trade->setTradeDate(timePoint);

    trade->setInstrument(items[2]+items[3]);
    trade->setNotional(std::stod(items[4]));
    trade->setRate(std::stod(items[5]));

    tm = {};
    std::istringstream valueDateStream(items[6]);
    valueDateStream >> std::get_time(&tm, "%Y-%m-%d");
    timePoint = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    trade->setValueDate(timePoint);

    trade->setCounterparty(items[7]);        

    return trade;
}
void FxTradeLoader::loadTradesFromFile(const std::string& filename, TradeList& tradeList) {
    if (filename.empty()) {
        throw std::invalid_argument("Filename cannot be null");
    }

    std::ifstream stream(filename);
    if (!stream.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    int lineCount = 0;
    std::string line;
    while (std::getline(stream, line)) {
        if (lineCount >= 2 && line.rfind("END", 0) != 0) {
            tradeList.add(createTradeFromLine(line));
        }
        lineCount++;
    }
}

std::vector<ITrade*> FxTradeLoader::loadTrades() {
    TradeList tradeList;
    loadTradesFromFile(dataFile_, tradeList);

    std::vector<ITrade*> result;
    for (size_t i = 0; i < tradeList.size(); ++i) {
        result.push_back(tradeList[i]);
    }
    return result;
}

std::string FxTradeLoader::getDataFile() const {
    return dataFile_;
}

void FxTradeLoader::setDataFile(const std::string& file) {
    dataFile_ = file;
}
