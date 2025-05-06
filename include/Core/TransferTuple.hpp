#pragma once

#include <utility>

class TransferTuple {
public:
    TransferTuple() = default;
    TransferTuple(int from, int to) : fromStation(from), toStation(to) {}
    
    int getFromStation() const { return fromStation; }
    int getToStation() const { return toStation; }
    
private:
    int fromStation;
    int toStation;
}; 