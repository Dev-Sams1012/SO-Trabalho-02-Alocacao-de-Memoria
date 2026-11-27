#pragma once

#include "i_estrategia_alocacao.hpp"

class FirstFit final : public IEstrategiaAlocacao {
public:
  ResultadoBusca buscar(const EstadoMemoria& estado,
                        std::size_t tamanho) const override;
};