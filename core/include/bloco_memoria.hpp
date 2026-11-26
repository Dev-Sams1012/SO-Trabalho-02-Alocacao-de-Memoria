#pragma once

#include <cstddef>

struct BlocoMemoria {
  std::size_t id{0};
  std::size_t inicio{0};
  std::size_t tamanho{0};
  
  bool estaAlocado{false};
};
