#pragma once

#include <map>
#include <string>

class SymbolRepository {
public:
    SymbolRepository();

    bool containsSymbol(const std::string& symbol) const;
    double resolveSymbol(const std::string& symbol) const;
    inline const std::map<std::string, double>& symbolTable() const {
        return _symbolTable;
    }
    inline void setSymbol(const std::string& symbol, double value) const {
        _symbolTable[symbol] = value;
    }

private:
    mutable std::map<std::string, double> _symbolTable;
};
