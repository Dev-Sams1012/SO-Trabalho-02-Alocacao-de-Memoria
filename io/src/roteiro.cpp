#include "roteiro.hpp"

#include <fstream>
#include <string_view>
#include <variant>

#include "interpretador.hpp"

namespace {

// Alguns editores do Windows gravam UTF-8 com BOM (3 bytes invisíveis no
// começo do arquivo). Sem remover, a primeira linha viraria "\xEF\xBB\xBFinit"
// e o comando não seria reconhecido.
void remover_bom(std::string& linha) {
  constexpr std::string_view bom = "\xEF\xBB\xBF";
  if (linha.starts_with(bom)) {
    linha.erase(0, bom.size());
  }
}

}  // namespace

std::expected<std::vector<PassoRoteiro>, std::string> carregar_roteiro(
    const std::filesystem::path& caminho) {
  std::ifstream arquivo(caminho);
  if (!arquivo) {
    return std::unexpected("Erro: não foi possível abrir o roteiro '" +
                           caminho.string() + "'");
  }

  std::vector<PassoRoteiro> passos;
  std::string linha;
  std::size_t numero = 0;

  while (std::getline(arquivo, linha)) {
    ++numero;
    if (numero == 1) remover_bom(linha);
    if (linha_ignoravel(linha)) continue;

    const auto comando = interpretar_linha(linha);
    if (!comando) {
      // Formato "arquivo:linha: erro", como os diagnósticos de compilador.
      return std::unexpected(caminho.filename().string() + ":" +
                             std::to_string(numero) + ": " + comando.error());
    }

    // show e stats não viram passos: a tela já exibe mapa e estatísticas.
    if (std::holds_alternative<ComandoShow>(*comando) ||
        std::holds_alternative<ComandoStats>(*comando)) {
      continue;
    }

    passos.push_back(
        PassoRoteiro{*comando, std::string(aparar(linha)), numero});
  }

  return passos;
}