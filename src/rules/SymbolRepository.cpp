#include "SymbolRepository.h"

#include <common/Logger.h>

SymbolRepository::SymbolRepository() {}

bool SymbolRepository::containsSymbol(const std::string& symbol) const {
    return _symbolTable.contains(symbol);
}

double SymbolRepository::resolveSymbol(const std::string& symbol) const {
    LOG_S(INFO) << "resolveSymbol: " << symbol;
    return _symbolTable[symbol];
}
