#pragma once

#include <cstddef>
#include <variant>

#include "fit_alg.hpp"
#include "option.hpp"

// Representação TIPADA dos comandos do simulador. O módulo io converte texto
// nestas structs; quem executa (CLI, simulação) trabalha só com elas e nunca
// compara strings.

struct ComandoInit {
  std::size_t tamanho{0};
};

struct ComandoAlloc {
  std::size_t tamanho{0};
  FitAlg algoritmo{FitAlg::FIRST};
};

struct ComandoFreeId {
  std::size_t id{0};
};

struct ComandoFreeAddr {
  std::size_t endereco{0};
};

struct ComandoDefrag {
  Option opcao{Option::INIT};
};

// show e stats não têm dados, mas precisam existir como comandos da CLI.
struct ComandoShow {};
struct ComandoStats {};

// Um comando é exatamente UM destes tipos. Quem executa usa std::visit, e o
// compilador avisa se um tipo novo for esquecido.
using Comando =
    std::variant<ComandoInit, ComandoAlloc, ComandoFreeId, ComandoFreeAddr,
                 ComandoDefrag, ComandoShow, ComandoStats>;
