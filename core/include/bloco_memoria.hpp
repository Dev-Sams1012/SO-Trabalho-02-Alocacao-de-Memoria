#pragma once

#include <cstddef>

// Um trecho contíguo da memória simulada. A memória inteira é descrita por um
// vetor de blocos ordenado por endereço, sem lacunas e sem sobreposição:
// cada bloco começa exatamente onde o anterior termina.
struct BlocoMemoria {
  // Identificador do bloco. Ids reais começam em 1 e nunca são reaproveitados.
  // Por convenção, um bloco LIVRE (buraco) tem id = 0.
  std::size_t id{0};

  // Endereço do primeiro byte do bloco.
  std::size_t inicio{0};

  // Quantidade de bytes do bloco.
  std::size_t tamanho{0};

  // true = bloco alocado; false = buraco.
  // Invariante: esta_alocado == (id != 0).
  bool esta_alocado{false};
};