#ifndef PARALLELPRICER_H
#define PARALLELPRICER_H

#include "../Models/IPricingEngine.h"
#include "../Models/ITrade.h"
#include "../Models/IScalarResultReceiver.h"
#include "PricingConfigLoader.h"
#include <map>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <future>

class ParallelPricer {
private:
    std::map<std::string, IPricingEngine*> pricers_;
    std::mutex resultMutex_;
    
    class ThreadSafeReceiver : public IScalarResultReceiver {
    public:
        ThreadSafeReceiver(IScalarResultReceiver* target, std::mutex* mutex)
            : target_(target), mutex_(mutex) {
        }

        void addResult(const std::string& tradeId, double result) override {
            std::lock_guard<std::mutex> lock(*mutex_);
            target_->addResult(tradeId, result);
        }

        void addError(const std::string& tradeId, const std::string& error) override {
            std::lock_guard<std::mutex> lock(*mutex_);
            target_->addError(tradeId, error);
        }

    private:
        IScalarResultReceiver* target_;
        std::mutex* mutex_;
    };

    void loadPricers();
    
public:
    ~ParallelPricer();
    
    void price(const std::vector<std::vector<ITrade*>>& tradeContainers, 
               IScalarResultReceiver* resultReceiver);

};

#endif // PARALLELPRICER_H
