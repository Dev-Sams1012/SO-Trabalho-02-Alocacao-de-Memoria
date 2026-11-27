#pragma once

#include <memory>

#include "fit_alg.hpp"
#include "i_estrategia_alocacao.hpp"

// Padrão Factory: transforma o FitAlg lido do comando (first/best/worst/next)
// na estratégia correspondente. Quem usa recebe apenas a interface.
class FabricaAlocadores {
public:
  // Lança std::invalid_argument se o valor não for um FitAlg conhecido.
  static std::unique_ptr<IEstrategiaAlocacao> criar(FitAlg algoritmo);
};