#pragma once

#include <cstddef>
#include <expected>
#include <filesystem>
#include <string>
#include <vector>

#include "comando.hpp"

// Um passo da simulação visual: um comando do roteiro, mais o texto original
// e a linha do arquivo (a tela mostra "o comando atual" e o passo N de M).
struct PassoRoteiro {
  Comando comando;
  std::string texto;   // linha original, sem espaços nas pontas
  std::size_t linha;   // número da linha no arquivo (começa em 1)
};

// Lê um arquivo de roteiro: um comando por linha, mesma sintaxe da CLI.
//  - linhas vazias e linhas que começam com '#' são ignoradas;
//  - show e stats são ignorados (a tela já mostra o mapa e as estatísticas);
//  - os demais comandos viram passos, NA ORDEM do arquivo.
// Se alguma linha for inválida, nada é carregado e a mensagem de erro traz
// "arquivo:linha: motivo". Um roteiro sem comandos é válido (lista vazia).
std::expected<std::vector<PassoRoteiro>, std::string> carregar_roteiro(
    const std::filesystem::path& caminho);