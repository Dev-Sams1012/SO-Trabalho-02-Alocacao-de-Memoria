#pragma once

#include <expected>
#include <string>
#include <string_view>

#include "comando.hpp"

// Remove espaços, tabs e quebras de linha das duas pontas (inclusive o '\r'
// que roteiros salvos no Windows deixam no fim da linha).
std::string_view aparar(std::string_view texto);

// true se a linha não tem comando: está vazia ou começa com '#'.
bool linha_ignoravel(std::string_view linha);

// Converte UMA linha de texto num comando tipado. Só valida a SINTAXE
// (quantidade de argumentos, números, nomes); regras de memória (id que não
// existe, falta de espaço...) são do GerenciadorMemoria.
// Nomes de comando, algoritmo e opção não diferenciam maiúsculas de minúsculas.
// Em caso de erro, devolve a mensagem pronta para exibir ("Erro: ...").
std::expected<Comando, std::string> interpretar_linha(std::string_view linha);