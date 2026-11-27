#include "best_fit.hpp"

#include "buracos.hpp"

// Best Fit: examina TODOS os buracos e usa o menor em que o pedido cabe.
// Em empate vale o de menor endereço: como percorremos em ordem crescente de
// endereço e só trocamos de escolha quando o buraco é ESTRITAMENTE menor,
// o primeiro entre os empatados é o que permanece.
ResultadoBusca BestFit::buscar(const EstadoMemoria& estado,
                               std::size_t tamanho) const {
  ResultadoBusca resultado;

  for (std::size_t i : indices_dos_buracos(estado)) {
    resultado.examinados.push_back(i);

    const BlocoMemoria& buraco = estado.blocos[i];
    if (!cabe(buraco, tamanho)) {
      continue;
    }

    if (!resultado.escolhido ||
        buraco.tamanho < estado.blocos[*resultado.escolhido].tamanho) {
      resultado.escolhido = i;
    }
  }

  return resultado;
}