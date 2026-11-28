#include "gerenciador_memoria.hpp"
#include "bloco_memoria.hpp"
#include "estado_memoria.hpp"

#include <algorithm>
#include <iomanip>
#include <ranges>
#include <sstream>
#include <string_view>
#include <utility>

GerenciadorMemoria::GerenciadorMemoria(FabricaEstrategia fabrica)
    : fabrica_(std::move(fabrica)) {}

std::expected<void, std::string> GerenciadorMemoria::init(std::size_t tamanho) {
  if (tamanho == 0) {
    return std::unexpected("O tamanho da memória deve ser maior que zero.");
  }

  EstadoMemoria novo_estado;

  novo_estado.tamanho_total = tamanho;

  BlocoMemoria bloco_inicial{.tamanho = tamanho};

  novo_estado.blocos.emplace_back(bloco_inicial);

  estado_ = std::move(novo_estado);

  return {};
}

EstatisticasMemoria GerenciadorMemoria::stats() const {
  EstatisticasMemoria estatisticas;

  std::size_t memoria_ocupada = std::ranges::fold_left(
      estado_.blocos, 0, [](std::size_t total, const BlocoMemoria &bloco) {
        return total + (bloco.esta_alocado ? bloco.tamanho : 0);
      });

  estatisticas.memoria_ocupada = memoria_ocupada;

  std::size_t memoria_livre = estado_.tamanho_total - memoria_ocupada;

  estatisticas.memoria_livre = memoria_livre;

  estatisticas.percentual_uso =
      static_cast<double>(memoria_ocupada) / estado_.tamanho_total * 100.0;

  std::size_t quantidade_buracos = std::ranges::count_if(
      estado_.blocos, [](bool esta_alocado) { return !esta_alocado; },
      &BlocoMemoria::esta_alocado);

  estatisticas.quantidade_buracos = quantidade_buracos;

  auto buracos =
      estado_.blocos | std::views::filter([](const BlocoMemoria &bloco) {
        return !bloco.esta_alocado;
      });

  auto maior_buraco =
      std::ranges::max_element(buracos, {}, &BlocoMemoria::tamanho);

  if (maior_buraco != buracos.end()) {
    estatisticas.maior_buraco = maior_buraco->tamanho;
  }

  return estatisticas;
}

std::string GerenciadorMemoria::show() const {
  if (estado_.tamanho_total == 0) {
    return "Memória não inicializada.\n";
  }

  std::string mapa_fisico(estado_.tamanho_total, '.');
  std::string mapa_ids(estado_.tamanho_total, '.');

  // Símbolos para representar os IDs na linha do mapa.
  constexpr std::string_view simbolos = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  for (const BlocoMemoria &bloco : estado_.blocos) {
    if (!bloco.esta_alocado) {
      continue;
    }

    char simbolo_id = bloco.id < simbolos.size() ? simbolos[bloco.id] : '?';

    for (std::size_t i = bloco.inicio; i < bloco.inicio + bloco.tamanho; ++i) {
      mapa_fisico[i] = '#';
      mapa_ids[i] = simbolo_id;
    }
  }

  const EstatisticasMemoria estatisticas = stats();

  std::ostringstream saida;

  const std::string separador(estado_.tamanho_total + 2, '-');

  saida << "Mapa de Memória (" << estado_.tamanho_total << " bytes)\n";
  saida << separador << '\n';

  saida << '[' << mapa_fisico << "]\n";
  saida << '[' << mapa_ids << "]\n";

  saida << separador << '\n';

  // Informações dos blocos alocados.
  saida << "Blocos ativos: ";

  bool primeiro = true;

  for (const BlocoMemoria &bloco : estado_.blocos) {
    if (!bloco.esta_alocado) {
      continue;
    }

    if (!primeiro) {
      saida << " | ";
    }

    saida << "[id=" << bloco.id << "] " << '@' << bloco.inicio << " +"
          << bloco.tamanho << "B"
          << " (usado=" << bloco.tamanho << "B)";

    primeiro = false;
  }

  if (primeiro) {
    saida << "Nenhum";
  }

  // Estatísticas.
  saida << "\n== Estatísticas ==\n";

  saida << "Tamanho total: " << estado_.tamanho_total << " bytes\n";

  saida << "Ocupado: " << estatisticas.memoria_ocupada
        << " bytes | Livre: " << estatisticas.memoria_livre << " bytes\n";

  saida << "Buracos (fragmentação externa): " << estatisticas.quantidade_buracos
        << '\n';

  saida << "Maior buraco livre: " << estatisticas.maior_buraco << " bytes\n";

  saida << "Uso efetivo: " << std::fixed << std::setprecision(2)
        << estatisticas.percentual_uso << "%\n";

  return saida.str();
}
