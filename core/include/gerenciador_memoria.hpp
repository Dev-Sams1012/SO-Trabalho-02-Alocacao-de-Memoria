#pragma once

#include <cstddef>
#include <expected>
#include <string>
#include <vector>

#include "bloco_memoria.hpp"
#include "estatisticas_memoria.hpp"
#include "fit_alg.hpp"
#include "option.hpp"
#include "resultado_desfragmentacao.hpp"

class GerenciadorMemoria {
public:
  std::expected<void, std::string> init(std::size_t tamanho);

  std::expected<void, std::string> alloc(std::size_t tamanho, FitAlg algoritmo);

  std::expected<void, std::string> free_id(std::size_t id);

  std::expected<void, std::string> free_addr(std::size_t endereco);

  std::expected<ResultadoDesfragmentacao, std::string> defrag(Option opcao);

  EstatisticasMemoria stats() const;

  std::string show() const;

private:
  std::vector<BlocoMemoria> blocos;

  std::size_t tamanho_total{0};
  std::size_t proximo_id{1};
  std::size_t ponteiro_next_fit{0};
};
