#pragma once

#include <cstddef>
#include <vector>

#include "bloco_memoria.hpp"

struct EstadoMemoria {
  std::vector<BlocoMemoria> blocos;

  std::size_t tamanho_total = 0;
  std::size_t proximo_id = 1;
  std::size_t ponteiro_next_fit = 0;
};