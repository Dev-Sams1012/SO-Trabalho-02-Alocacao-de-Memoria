#pragma once

#include <cstddef>
#include <expected>
#include <functional>
#include <memory>
#include <string>

#include "estado_memoria.hpp"
#include "estatisticas_memoria.hpp"
#include "fit_alg.hpp"
#include "i_estrategia_alocacao.hpp"
#include "option.hpp"
#include "resultado_alocacao.hpp"
#include "resultado_desfragmentacao.hpp"

// Única classe que modifica o EstadoMemoria.
class GerenciadorMemoria {
public:
  // Função que cria a estratégia correspondente a um FitAlg. Quem monta o
  // programa (CLI/TUI) injeta aqui a FabricaAlocadores, assim o core não
  // precisa conhecer as classes concretas (inversão de dependência).
  using FabricaEstrategia =
      std::function<std::unique_ptr<IEstrategiaAlocacao>(FitAlg)>;

  explicit GerenciadorMemoria(FabricaEstrategia fabrica);

  std::expected<void, std::string> init(std::size_t tamanho);

  std::expected<ResultadoAlocacao, std::string> alloc(std::size_t tamanho,
                                                      FitAlg algoritmo);

  std::expected<void, std::string> free_id(std::size_t id);

  std::expected<void, std::string> free_addr(std::size_t endereco);

  std::expected<ResultadoDesfragmentacao, std::string> defrag(Option opcao);

  EstatisticasMemoria stats() const;

  std::string show() const;

  const EstadoMemoria& estado() const;

  void restaurar(const EstadoMemoria& estado);

private:
  EstadoMemoria estado_;
  FabricaEstrategia fabrica_;
};