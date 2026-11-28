#include "worst_fit.hpp"

#include "buracos.hpp"

// Worst Fit: examina TODOS os buracos e usa o maior em que o pedido cabe.
// Mesma lógica de desempate do Best Fit, mas trocando a escolha somente
// quando o buraco é ESTRITAMENTE maior (empate fica com o menor endereço).
ResultadoBusca WorstFit::buscar(const EstadoMemoria& estado,
                                std::size_t tamanho) const {
  ResultadoBusca resultado;
  std::size_t maior_tamanho = 0;

  for (std::size_t i : indices_dos_buracos(estado)) {
    resultado.examinados.push_back(i);

    const BlocoMemoria& buraco = estado.blocos[i];
    if (!cabe(buraco, tamanho)) {
      continue;
    }

    if (!resultado.escolhido || buraco.tamanho > maior_tamanho) {
      resultado.escolhido = i;
      maior_tamanho = buraco.tamanho;
    }
  }

  return resultado;
}
