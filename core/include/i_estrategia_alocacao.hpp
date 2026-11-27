#pragma once

#include <cstddef>
#include <optional>
#include <vector>

#include "estado_memoria.hpp"

// Resultado de uma busca por buraco, feita por uma estratégia.
struct ResultadoBusca {
  // Índice (em estado.blocos) do buraco escolhido; nullopt se nenhum coube.
  std::optional<std::size_t> escolhido;

  // Índices (em estado.blocos) dos buracos examinados, na ordem da busca.
  std::vector<std::size_t> examinados;
};

// Interface do padrão Strategy: cada algoritmo (First, Best, Worst, Next Fit)
// só decide QUAL buraco usar. Dividir o buraco, gerar o id e mover o ponteiro
// do Next Fit continuam sendo responsabilidade do GerenciadorMemoria.
//
// A interface fica no core porque ela pertence a quem a usa (o gerenciador);
// as implementações concretas ficam no módulo alocadores.
class IEstrategiaAlocacao {
public:
  virtual ~IEstrategiaAlocacao() = default;

  // Somente lê o estado: nunca altera blocos, ids nem o ponteiro do Next Fit.
  virtual ResultadoBusca buscar(const EstadoMemoria& estado,
                                std::size_t tamanho) const = 0;
};