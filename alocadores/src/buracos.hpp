#pragma once

// Funções auxiliares INTERNAS do módulo alocadores (não fazem parte da API
// pública: por isso ficam em src/ e não em include/).

#include <cstddef>
#include <vector>

#include "estado_memoria.hpp"

// Índices, em estado.blocos, de todos os buracos, em ordem crescente de
// endereço. Como o vetor de blocos já é mantido ordenado por endereço, basta
// percorrê-lo uma vez.
inline std::vector<std::size_t> indices_dos_buracos(const EstadoMemoria& estado) {
  std::vector<std::size_t> buracos;
  for (std::size_t i = 0; i < estado.blocos.size(); ++i) {
    if (!estado.blocos[i].esta_alocado) {
      buracos.push_back(i);
    }
  }
  return buracos;
}

// Um pedido de `tamanho` bytes cabe no bloco (buraco)?
inline bool cabe(const BlocoMemoria& buraco, std::size_t tamanho) {
  return buraco.tamanho >= tamanho;
}