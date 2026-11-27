#pragma once

#include <cstddef>
#include <vector>

// Dados de uma alocação bem-sucedida. É informação do domínio (não depende de
// nenhuma interface): a simulação usa isso para destacar o que a busca fez.
struct ResultadoAlocacao {
  // Bloco criado.
  std::size_t id{0};
  std::size_t inicio{0};
  std::size_t tamanho{0};

  // Endereço inicial do buraco onde o bloco foi colocado.
  std::size_t buraco_escolhido{0};

  // Endereços iniciais dos buracos examinados pela busca, na ordem em que
  // foram olhados (o escolhido é o último da lista). Usamos endereços, e não
  // índices do vetor, porque os índices mudam quando o buraco é dividido.
  std::vector<std::size_t> buracos_examinados;
};