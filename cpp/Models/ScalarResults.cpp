#include "ScalarResults.h"
#include <stdexcept>
#include <set>

ScalarResults::~ScalarResults() = default;

std::optional<ScalarResult> ScalarResults::operator[](const std::string& tradeId) const {
    if (!containsTrade(tradeId)) {
        return std::nullopt;
    }

    std::optional<double> priceResult = std::nullopt;
    std::optional<std::string> error = std::nullopt;

    auto resultIt = results_.find(tradeId);
    if (resultIt != results_.end()) {
        priceResult = resultIt->second;
    }

    auto errorIt = errors_.find(tradeId);
    if (errorIt != errors_.end()) {
        error = errorIt->second;
    }

    return ScalarResult(tradeId, priceResult, error);
}

bool ScalarResults::containsTrade(const std::string& tradeId) const {
    return results_.find(tradeId) != results_.end() || errors_.find(tradeId) != errors_.end();
}

void ScalarResults::addResult(const std::string& tradeId, double result) {
    results_[tradeId] = result;
}

void ScalarResults::addError(const std::string& tradeId, const std::string& error) {
    errors_[tradeId] = error;
}

ScalarResults::Iterator::Iterator(const ScalarResults* parent, const std::vector<std::string>& ids, size_t index)
    : parent_(parent), ids_(ids), index_(index) {
}

ScalarResults::Iterator& ScalarResults::Iterator::operator++() {
    index_++;
    return *this;
}

ScalarResult ScalarResults::Iterator::operator*() const {
    return (*parent_)[ids_[index_]].value();
}

bool ScalarResults::Iterator::operator!=(const Iterator& other) const {
    return (index_ != other.index_);
}

ScalarResults::Iterator ScalarResults::begin() const {
    std::vector<std::string> ids = getTradeIds();
    return Iterator(this, ids, 0);
}

ScalarResults::Iterator ScalarResults::end() const {
    std::vector<std::string> ids = getTradeIds();
    return Iterator(this, ids, ids.size());
}

std::vector<std::string> ScalarResults::getTradeIds() const {
    std::set<std::string> ids;
    for (const auto& pair : results_) ids.insert(pair.first);
    for (const auto& pair : errors_) ids.insert(pair.first);
    return std::vector<std::string>(ids.begin(), ids.end());
}