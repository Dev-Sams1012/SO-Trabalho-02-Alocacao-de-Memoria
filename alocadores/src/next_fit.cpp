#include "next_fit.hpp"

#include "buracos.hpp"

// Next Fit: a busca começa perto do ponteiro (endereço logo após o último
// bloco alocado) em vez de recomeçar sempre do início da memória.
//
// Regras do enunciado:
//  1. Começa no primeiro buraco cujo FIM é maior que o ponteiro (o buraco que
//     contém o ponteiro é examinado inteiro).
//  2. Segue em ordem crescente de endereço; ao chegar ao fim da memória, volta
//     ao endereço 0 e continua até esgotar os buracos.
//  3. Usa o primeiro buraco em que o pedido cabe.
//
// A estratégia NÃO guarda o ponteiro: ela só o lê de estado.ponteiro_next_fit.
// Quem o atualiza é o GerenciadorMemoria, depois de uma alocação bem-sucedida.
ResultadoBusca NextFit::buscar(const EstadoMemoria& estado,
                               std::size_t tamanho) const {
  ResultadoBusca resultado;

  const std::vector<std::size_t> buracos = indices_dos_buracos(estado);
  const std::size_t n = buracos.size();

  // Posição (dentro de `buracos`) do primeiro buraco com fim > ponteiro.
  // Se nenhum satisfaz (ex.: ponteiro no fim da memória), p = n e a busca
  // começa direto pelo início, que é o "volta ao endereço 0" da regra 2.
  std::size_t p = 0;
  while (p < n) {
    const BlocoMemoria& buraco = estado.blocos[buracos[p]];
    if (buraco.inicio + buraco.tamanho > estado.ponteiro_next_fit) {
      break;
    }
    ++p;
  }

  // Visita cada buraco exatamente uma vez: de p até o fim e, depois, do
  // início até p-1. O "% n" faz a volta circular.
  for (std::size_t k = 0; k < n; ++k) {
    const std::size_t posicao = (p + k) % n;
    const std::size_t indice_bloco = buracos[posicao];
    
    resultado.examinados.push_back(indice_bloco);
    
    if (cabe(estado.blocos[indice_bloco], tamanho)) {
      resultado.escolhido = indice_bloco;
      break;
    }
  }

  return resultado;
}