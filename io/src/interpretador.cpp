#include "interpretador.hpp"

#include <cctype>
#include <charconv>
#include <string>
#include <vector>

namespace {

bool eh_espaco(char c) {
  return std::isspace(static_cast<unsigned char>(c)) != 0;
}

// Quebra a linha em palavras separadas por espaços/tabs, sem copiar texto.
std::vector<std::string_view> dividir(std::string_view texto) {
  std::vector<std::string_view> partes;
  std::size_t i = 0;
  while (i < texto.size()) {
    while (i < texto.size() && eh_espaco(texto[i])) ++i;
    const std::size_t inicio = i;
    while (i < texto.size() && !eh_espaco(texto[i])) ++i;
    if (i > inicio) partes.push_back(texto.substr(inicio, i - inicio));
  }
  return partes;
}

std::string minusculas(std::string_view texto) {
  std::string resultado(texto);
  for (char& c : resultado) {
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  }
  return resultado;
}

// Lê um inteiro sem sinal. Rejeita "-5", "+5", "10abc", "abc", "" e valores
// que não cabem em size_t. Com `positivo`, rejeita também o zero.
std::expected<std::size_t, std::string> ler_inteiro(std::string_view texto,
                                                    const std::string& nome,
                                                    bool positivo) {
  std::size_t valor = 0;
  const char* fim = texto.data() + texto.size();
  const auto [ptr, erro] = std::from_chars(texto.data(), fim, valor);

  if (erro == std::errc::result_out_of_range) {
    return std::unexpected("Erro: " + nome + " muito grande: '" +
                           std::string(texto) + "'");
  }
  // ptr != fim pega lixo no fim do número, como "10abc".
  if (erro != std::errc{} || ptr != fim) {
    return std::unexpected("Erro: " + nome + " inválido: '" +
                           std::string(texto) + "' (use um inteiro " +
                           (positivo ? "positivo" : "não negativo") + ")");
  }
  if (positivo && valor == 0) {
    return std::unexpected("Erro: " + nome + " deve ser maior que zero");
  }
  return valor;
}

std::expected<FitAlg, std::string> ler_algoritmo(std::string_view texto) {
  const std::string nome = minusculas(texto);
  if (nome == "first") return FitAlg::FIRST;
  if (nome == "best") return FitAlg::BEST;
  if (nome == "worst") return FitAlg::WORST;
  if (nome == "next") return FitAlg::NEXT;
  return std::unexpected("Erro: algoritmo desconhecido: '" +
                         std::string(texto) +
                         "' (use first, best, worst ou next)");
}

std::expected<Option, std::string> ler_opcao(std::string_view texto) {
  const std::string nome = minusculas(texto);
  if (nome == "init") return Option::INIT;
  if (nome == "end") return Option::END;
  if (nome == "opt") return Option::OPT;
  return std::unexpected("Erro: opção de defrag desconhecida: '" +
                         std::string(texto) + "' (use init, end ou opt)");
}

std::unexpected<std::string> uso(const std::string& sintaxe) {
  return std::unexpected("Erro: uso: " + sintaxe);
}

}  // namespace

std::string_view aparar(std::string_view texto) {
  while (!texto.empty() && eh_espaco(texto.front())) texto.remove_prefix(1);
  while (!texto.empty() && eh_espaco(texto.back())) texto.remove_suffix(1);
  return texto;
}

bool linha_ignoravel(std::string_view linha) {
  const std::string_view limpa = aparar(linha);
  return limpa.empty() || limpa.front() == '#';
}

std::expected<Comando, std::string> interpretar_linha(std::string_view linha) {
  const std::vector<std::string_view> partes = dividir(linha);
  if (partes.empty()) {
    return std::unexpected("Erro: comando vazio");
  }

  const std::string nome = minusculas(partes[0]);
  const std::size_t n_args = partes.size() - 1;

  if (nome == "init") {
    if (n_args != 1) return uso("init <tamanho>");
    const auto tamanho = ler_inteiro(partes[1], "tamanho", true);
    if (!tamanho) return std::unexpected(tamanho.error());
    return ComandoInit{*tamanho};
  }

  if (nome == "alloc") {
    if (n_args != 2) return uso("alloc <tamanho> <first|best|worst|next>");
    const auto tamanho = ler_inteiro(partes[1], "tamanho", true);
    if (!tamanho) return std::unexpected(tamanho.error());
    const auto algoritmo = ler_algoritmo(partes[2]);
    if (!algoritmo) return std::unexpected(algoritmo.error());
    return ComandoAlloc{*tamanho, *algoritmo};
  }

  if (nome == "freeid") {
    if (n_args != 1) return uso("freeid <id>");
    const auto id = ler_inteiro(partes[1], "id", false);
    if (!id) return std::unexpected(id.error());
    return ComandoFreeId{*id};
  }

  if (nome == "freeaddr") {
    if (n_args != 1) return uso("freeaddr <endereço>");
    const auto endereco = ler_inteiro(partes[1], "endereço", false);
    if (!endereco) return std::unexpected(endereco.error());
    return ComandoFreeAddr{*endereco};
  }

  if (nome == "defrag") {
    if (n_args != 1) return uso("defrag <init|end|opt>");
    const auto opcao = ler_opcao(partes[1]);
    if (!opcao) return std::unexpected(opcao.error());
    return ComandoDefrag{*opcao};
  }

  if (nome == "show" || nome == "stats") {
    if (n_args != 0) return uso(nome);
    if (nome == "show") return ComandoShow{};
    return ComandoStats{};
  }

  return std::unexpected("Erro: comando desconhecido: '" +
                         std::string(partes[0]) + "'");
}