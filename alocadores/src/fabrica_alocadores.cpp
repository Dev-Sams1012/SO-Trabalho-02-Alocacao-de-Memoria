#include "fabrica_alocadores.hpp"

#include <stdexcept>

#include "best_fit.hpp"
#include "first_fit.hpp"
#include "next_fit.hpp"
#include "worst_fit.hpp"

std::unique_ptr<IEstrategiaAlocacao> FabricaAlocadores::criar(FitAlg algoritmo) {
  switch (algoritmo) {
    case FitAlg::FIRST:
      return std::make_unique<FirstFit>();
    case FitAlg::BEST:
      return std::make_unique<BestFit>();
    case FitAlg::WORST:
      return std::make_unique<WorstFit>();
    case FitAlg::NEXT:
      return std::make_unique<NextFit>();
  }

  // Só chega aqui se alguém passar um valor inválido via cast.
  throw std::invalid_argument("FitAlg desconhecido");
}