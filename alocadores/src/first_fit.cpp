#include "first_fit.hpp"

#include "buracos.hpp"

// First Fit: percorre os buracos em ordem crescente de endereço e usa o
// primeiro em que o pedido cabe. A busca para nele.
ResultadoBusca FirstFit::buscar(const EstadoMemoria& estado,
                                std::size_t tamanho) const {
  ResultadoBusca resultado;

  for (std::size_t i : indices_dos_buracos(estado)) {
    resultado.examinados.push_back(i);

    if (cabe(estado.blocos[i], tamanho)) {
      resultado.escolhido = i;
      break;
    }
  }

  return resultado;
}