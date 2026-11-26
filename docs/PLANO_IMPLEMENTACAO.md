# Plano de implementação — Simulador de Alocação de Memória

> Plano de trabalho para desenvolvimento em dupla. A sequência considera o estado atual do repositório e organiza as entregas em commits pequenos, com tarefas paralelas sempre que não houver dependência direta entre elas.

**Repositório:** [SO-Trabalho-02-Alocacao-de-Memoria](https://github.com/Dev-Sams1012/SO-Trabalho-02-Alocacao-de-Memoria)  
**Linguagem:** C++23  
**Build:** CMake  
**Equipe:** Samuel Augusto de Abreu e Jonathan Duarte Uchoa

---

## 1. Estado atual do projeto

O planejamento parte do conteúdo atualmente publicado na branch `main`.

### Já existe

- Configuração inicial do CMake com C++23 e exportação de `compile_commands.json`.
- Configuração do `clangd` e `.gitignore`.
- Estruturas `BlocoMemoria`, `EstadoMemoria`, `EstatisticasMemoria` e `ResultadoDesfragmentacao`.
- Enumeradores `FitAlg` e `Option`.
- Declaração de `GerenciadorMemoria` com operações de inicialização, alocação, liberação, desfragmentação, consulta e restauração.
- `GerenciadorMemoria` já mantém um único objeto `EstadoMemoria estado_`, em vez de duplicar seus campos. Também existem `estado()` e `restaurar()`, que servirão de base para o histórico da simulação.

### Ainda precisa ser desenvolvido

- Implementações `.cpp` do núcleo.
- Interface Strategy, algoritmos concretos e Factory.
- Leitura e interpretação de comandos e roteiros.
- Executor comum para CLI e simulação.
- Histórico e execução passo a passo.
- Interface de terminal e interface gráfica.
- Testes automatizados, exemplos de roteiros e documentação de arquitetura.

**Observação de consistência:** o CMake já configura C++23, mas a descrição pública do repositório ainda menciona C++20. Atualizar essa descrição e manter README/CMake alinhados em um dos commits de documentação.

---

## 2. Organização pretendida

A estrutura abaixo é uma referência para os próximos módulos. Não é necessário criar todos os arquivos vazios de uma vez: cada commit deve adicionar os arquivos quando a responsabilidade correspondente começar a ser implementada.

```text
SO-Trabalho-02-Alocacao-de-Memoria/
├── CMakeLists.txt
├── README.md
├── .clangd
├── .gitignore
├── core/
│   ├── CMakeLists.txt
│   ├── include/
│   └── src/
├── alocadores/
│   ├── CMakeLists.txt
│   ├── include/
│   └── src/
├── io/
│   ├── CMakeLists.txt
│   ├── include/
│   └── src/
├── simulacao/
│   ├── CMakeLists.txt
│   ├── include/
│   └── src/
├── cli/
│   ├── CMakeLists.txt
│   └── src/
├── app/
│   ├── CMakeLists.txt
│   ├── include/
│   └── src/
├── tests/
├── scripts/
└── docs/
    └── arquitetura.md
```

### Responsabilidade de cada módulo

| Módulo | Responsabilidade | Não deve fazer |
|---|---|---|
| `core` | Estado da memória, blocos, gerenciamento, estatísticas e desfragmentação. | Interpretar texto, imprimir no terminal ou conhecer componentes gráficos. |
| `alocadores` | Implementar First Fit, Best Fit, Worst Fit e Next Fit, além da Factory. | Alterar diretamente a memória ou controlar a interface gráfica. |
| `io` | Interpretar comandos, validar argumentos textuais e carregar roteiros. | Reimplementar regras de alocação. |
| `simulacao` | Executar comandos, registrar histórico e controlar os passos da simulação. | Desenhar diretamente os componentes visuais. |
| `cli` | Interface de terminal que recebe comandos e apresenta resultados. | Duplicar as regras do `core`. |
| `app` | Interface gráfica, mapa de memória, controles e apresentação das alterações. | Implementar algoritmos de alocação ou desfragmentação. |
| `tests` | Testes unitários e de integração. | Ser dependência da aplicação principal. |
| `scripts` | Roteiros de entrada para testar e demonstrar o simulador. | Conter lógica de execução. |
| `docs` | Decisões arquiteturais e documentação complementar. | Substituir o README de execução. |

### Direção das dependências

A regra principal é manter o `core` independente das interfaces. Os algoritmos concretos podem depender dos tipos do `core`, mas o `core` não deve incluir classes concretas de `alocadores`. A CLI e a GUI devem reutilizar os mesmos módulos de execução.

```text
alocadores ───────► core
io ───────────────► tipos de comandos
simulacao ────────► core + io + alocadores
cli ──────────────► io + simulacao
app ──────────────► io + simulacao
```

A seta indica dependência de código. O CMake deve refletir essa direção, evitando dependências circulares entre bibliotecas.

---

## 3. Design patterns e decisões de arquitetura

### 3.1 Strategy — principal padrão do projeto

Os quatro algoritmos de alocação têm o mesmo objetivo — encontrar um buraco adequado — mas seguem critérios diferentes. Isso é um caso direto para o padrão **Strategy**.

Proposta:

- Criar uma interface `IEstrategiaAlocacao`.
- Implementar `FirstFit`, `BestFit`, `WorstFit` e `NextFit` separadamente.
- Fazer o `GerenciadorMemoria` utilizar a abstração, sem conhecer as classes concretas.
- Manter cada estratégia focada na busca do buraco. A alteração dos blocos, a geração do ID e a atualização do estado continuam sob responsabilidade do `GerenciadorMemoria`.

A interface deve receber informações suficientes para executar a busca. Como a interface gráfica precisa destacar os buracos examinados, o resultado da busca também deve informar quais buracos foram analisados e qual foi escolhido. Um contrato possível é retornar o índice do buraco selecionado e uma sequência de índices examinados.

**Regra importante para o Next Fit:** a estratégia deve ser sem estado mutável próprio. O ponteiro Next Fit pertence ao `EstadoMemoria`, que já possui `ponteiro_next_fit`. O valor atual é passado à estratégia; o `GerenciadorMemoria` o atualiza somente quando uma alocação é concluída com sucesso. Assim, `EstadoMemoria` continua sendo suficiente para restaurar o comportamento exato da simulação.

### 3.2 Factory — criação das estratégias

A `FabricaAlocadores` transforma um valor de `FitAlg` na estratégia correspondente, retornando uma abstração, por exemplo `std::unique_ptr<IEstrategiaAlocacao>`.

A Factory deve ser usada pela camada que interpreta/executa o comando `alloc`. Dessa forma, o `core` não precisa conhecer `FirstFit`, `BestFit`, `WorstFit` ou `NextFit` concretamente.

### 3.3 Memento — voltar passos na simulação

O padrão **Memento** é representado pelo uso de `EstadoMemoria` como fotografia restaurável do estado do gerenciador.

- Antes de executar cada comando da simulação, salvar o estado necessário para restauração.
- Ao voltar, chamar `restaurar()` com o estado anterior.
- O snapshot precisa preservar blocos, tamanho total, próximo ID e ponteiro Next Fit.
- Não guardar apenas o desenho da memória: restaurar os dados reais para que os próximos comandos tenham o mesmo resultado.

A estrutura atual de `EstadoMemoria`, junto de `estado()` e `restaurar()`, já oferece o ponto de partida para isso.

### 3.4 Command — representação e execução dos comandos

Os comandos `init`, `alloc`, `freeid`, `freeaddr` e `defrag` devem ser convertidos pelo módulo `io` em uma representação tipada, em vez de espalhar comparações de strings pela CLI e pela GUI.

Um executor compartilhado recebe essa representação e chama o `GerenciadorMemoria`. A CLI e a simulação utilizam o mesmo executor, evitando que uma interface tenha regras diferentes da outra. Essa organização pode usar o padrão **Command** com objetos de comando ou uma representação tipada com `std::variant`; escolher a forma mais simples que atenda ao projeto.

### 3.5 Observer — opcional

Não implementar Observer só por implementar. Inicialmente, a GUI pode consultar o resultado da operação e o estado atualizado depois de cada passo. Considerar Observer apenas se a atualização de vários componentes ficar muito acoplada ao controlador da interface.

---

## 4. Contratos que precisam ser combinados antes do trabalho paralelo

Antes de cada integrante implementar seu módulo, alinhar estes pontos em conjunto e registrar as assinaturas nos headers:

1. **Representação da memória:** os blocos ficam ordenados por endereço e representam toda a faixa de memória. Blocos livres adjacentes devem ser unidos após uma liberação. Definir claramente como identificar um bloco livre e como representar seu ID.
2. **Contrato de Strategy:** definir os dados de entrada e o resultado da busca. O resultado precisa permitir exibir os buracos examinados e o buraco escolhido.
3. **Responsabilidade do gerenciador:** somente `GerenciadorMemoria` modifica o estado, cria IDs, divide buracos, libera blocos e atualiza o ponteiro Next Fit.
4. **Resultado de `alloc`:** a assinatura atual retorna `std::expected<void, std::string>`. Isso é suficiente para comunicar erros, mas não entrega à simulação os dados da busca. Recomenda-se avaliar um retorno como `std::expected<ResultadoAlocacao, std::string>`, contendo os dados do bloco alocado e os buracos examinados. A informação é do domínio, não deve depender da GUI.
5. **Comandos tipados:** definir uma representação comum para os comandos e o contrato do executor antes de implementar CLI e simulação.
6. **Framework gráfico:** escolher juntos a biblioteca da GUI e a forma de integrá-la ao CMake antes de criar componentes visuais. Não deixar essa decisão para depois do desenvolvimento do simulador.
7. **C++23 e nomes:** manter C++23 e a convenção de nomes em português/snake_case nos novos tipos, métodos e campos. Como ajuste de consistência, renomear `estaAlocado` para `esta_alocado`, preservando os nomes exigidos pelo enunciado (`init`, `alloc`, `free_id`, `free_addr`, `defrag`, `stats`, `show`, `FitAlg` e `Option`).

---

## 5. Plano de commits e divisão da dupla

Os commits abaixo começam depois do estado atualmente publicado. O primeiro registra este plano; os demais implementam o sistema. As tarefas marcadas como paralelas podem ser desenvolvidas em branches separadas e integradas quando ambas estiverem prontas.

### Commit 5 — `docs: adiciona plano de implementação`

**Responsável:** Samuel  
**Entrega:** adicionar este arquivo ao repositório, preferencialmente em `docs/PLANO_IMPLEMENTACAO.md`.

### Commit 6 — `build: organiza estrutura modular do projeto`

**Trabalho conjunto — etapa de fundação.**

- Criar a estrutura de diretórios dos módulos.
- Organizar o CMake raiz e os `CMakeLists.txt` dos módulos.
- Definir bibliotecas para `core`, `alocadores`, `io` e `simulacao`, além dos executáveis `simulador_cli` e `simulador_gui`.
- Criar uma opção como `BUILD_GUI` para permitir compilar a CLI sem depender da biblioteca gráfica.
- Habilitar testes com CTest.
- Fechar e documentar os contratos de Strategy, resultado da alocação, comandos tipados e executor compartilhado.
- Confirmar a biblioteca gráfica e registrar sua dependência no CMake.

**Critério de conclusão:** o projeto configura e compila os alvos ainda que algumas implementações estejam temporariamente vazias ou em construção. Nenhum módulo deve depender da GUI para compilar o `core`.

### Commit 7A — `feat(core): implementa inicialização e consultas da memória`

**Responsável:** Samuel  
**Pode ocorrer em paralelo com o Commit 7B.**

- Implementar `init()` e a criação de um único bloco livre.
- Garantir a reinicialização de `proximo_id` e `ponteiro_next_fit`.
- Implementar `stats()` e o cálculo do percentual de uso.
- Implementar `show()` com o mapa textual e as informações exigidas.
- Criar testes para inicialização, memória livre/ocupada e estatísticas.
- Estabelecer e validar as invariantes da representação interna dos blocos.

### Commit 7B — `feat(alocadores): implementa Strategy e Factory`

**Responsável:** Jonathan  
**Pode ocorrer em paralelo com o Commit 7A, após o contrato comum do Commit 6.**

- Implementar `IEstrategiaAlocacao` conforme o contrato acordado.
- Implementar `FirstFit`, `BestFit`, `WorstFit` e `NextFit`.
- Implementar `FabricaAlocadores` para criar a estratégia a partir de `FitAlg`.
- Garantir desempate pelo menor endereço em First/Best/Worst Fit.
- Implementar a busca circular do Next Fit, respeitando o ponteiro e o wrap para o início.
- Retornar os buracos examinados e o escolhido para a futura visualização.
- Criar testes unitários para cada estratégia, incluindo empates e casos de wrap do Next Fit.

### Commit 8A — `feat(core): implementa alocação de blocos`

**Responsável:** Samuel  
**Pode ocorrer em paralelo com o Commit 8B.**

- Integrar o `GerenciadorMemoria` à interface Strategy.
- Alocar sempre no início do buraco escolhido.
- Dividir o buraco quando houver espaço restante.
- Gerar IDs sequenciais sem reaproveitamento.
- Atualizar o ponteiro Next Fit somente após alocação bem-sucedida.
- Não alterar o estado, o ID ou o ponteiro em caso de falha.
- Retornar os dados necessários para a simulação destacar a busca e o bloco criado.
- Implementar a mensagem de erro especificada pelo enunciado, incluindo livre total e maior buraco.

### Commit 8B — `feat(core): implementa liberação e coalescência`

**Responsável:** Jonathan  
**Pode ocorrer em paralelo com o Commit 8A, mantendo as alterações em arquivos `.cpp` separados.**

- Implementar `free_id()` e `free_addr()`.
- Validar ID inexistente e endereço que não seja o início de um bloco alocado.
- Marcar o bloco como livre.
- Unir buracos livres adjacentes, inclusive quando houver dois vizinhos livres.
- Manter a ordenação e a cobertura contínua da memória.
- Criar testes de liberação, erros e coalescência à esquerda, à direita e dos dois lados.

### Commit 9A — `feat(core): implementa desfragmentação`

**Responsável:** Samuel  
**Pode ocorrer em paralelo com o Commit 9B.**

- Implementar `defrag(Option::INIT)`, deixando o espaço livre no início.
- Implementar `defrag(Option::END)`, deixando o espaço livre no fim.
- Implementar `defrag(Option::OPT)`, avaliando cada divisão `k`: os `k` primeiros blocos ficam agrupados no início e os demais no fim, deixando um único buraco entre eles.
- Escolher a divisão que minimiza a soma dos tamanhos dos blocos movidos; em empate, escolher o maior `k`.
- Preservar ordem relativa e IDs dos blocos alocados.
- Contabilizar blocos movidos e soma dos tamanhos dos blocos movidos.
- Em empate na opção OPT, escolher o maior `k`, conforme o enunciado.
- Reiniciar o ponteiro Next Fit para zero após desfragmentar.
- Criar testes para INIT, END, OPT, empates e contagem de movimentação.

### Commit 9B — `feat(io): implementa interpretação de comandos e roteiros`

**Responsável:** Jonathan  
**Pode ocorrer em paralelo com o Commit 9A.**

- Implementar a interpretação de `init`, `alloc`, `freeid`, `freeaddr` e `defrag`.
- Interpretar também `show` e `stats` para uso na CLI.
- Validar quantidade de argumentos, números, valores negativos/inválidos e nomes de algoritmos/opções.
- Implementar leitura de arquivo de roteiro.
- Ignorar linhas vazias, comentários iniciados por `#`, `show` e `stats` no modo de roteiro visual.
- Preservar a ordem dos demais comandos.
- Criar roteiros pequenos para First Fit, Next Fit, coalescência, falha de alocação e desfragmentação.
- Criar testes do parser e do carregador de roteiros.

### Commit 10A — `feat(simulacao): implementa histórico e execução passo a passo`

**Responsável:** Samuel  
**Pode ocorrer em paralelo com o Commit 10B após o contrato do executor estar definido.**

- Implementar `HistoricoSimulacao` com snapshots de `EstadoMemoria`.
- Implementar a execução de um comando por passo.
- Implementar Avançar, Voltar e Reiniciar.
- Garantir que Voltar restaure blocos, tamanho total, próximo ID e ponteiro Next Fit.
- Tratar falhas como passos executados, sem modificar o estado da memória.
- Expor o comando atual, número do passo, total de passos, resultado da operação e informações necessárias à GUI.
- Criar testes de avanço, retorno, reinício e restauração exata do estado.

### Commit 10B — `feat(cli): implementa interface de terminal`

**Responsável:** Jonathan  
**Pode ocorrer em paralelo com o Commit 10A se o contrato do executor estiver pronto.**

- Implementar o loop de leitura de comandos no terminal.
- Usar o interpretador e o executor compartilhado, sem duplicar regras do `core`.
- Apresentar mensagens de sucesso e erro.
- Implementar a apresentação de `show` e `stats`.
- Permitir executar comandos diretamente no terminal, usando a mesma sintaxe prevista para os roteiros. A leitura de arquivo pela CLI pode ser adicionada como funcionalidade extra, se houver tempo.
- Testar sequências de comandos e entradas inválidas.

### Commit 11A — `feat(app): implementa visualização do mapa de memória`

**Responsável:** Samuel  
**Pode ocorrer em paralelo com o Commit 11B, usando componentes/arquivos separados.**

- Criar a janela principal e a área de visualização da memória.
- Desenhar blocos alocados e buracos com cores distintas.
- Exibir escala de endereços, ID, endereço inicial e tamanho dos blocos.
- Exibir estatísticas de memória.
- Destacar buracos examinados e buraco escolhido durante uma alocação.
- Destacar blocos liberados, coalescência e blocos movidos na desfragmentação.
- Exibir o ponteiro Next Fit quando o algoritmo for utilizado.

### Commit 11B — `feat(app): implementa controles da simulação`

**Responsável:** Jonathan  
**Pode ocorrer em paralelo com o Commit 11A, usando componentes/arquivos separados.**

- Implementar seleção e carregamento de arquivo de roteiro.
- Implementar botões Avançar, Voltar, Reproduzir/Pausar e Reiniciar.
- Implementar reprodução automática com intervalo decimal em segundos.
- Garantir que avançar ou voltar manualmente pause a reprodução automática.
- Exibir comando atual, progresso e mensagens de erro.
- Conectar os controles ao módulo `simulacao`, sem executar regras de memória diretamente na GUI.

### Commit 12 — `test: integra e valida os fluxos completos`

**Responsáveis:** Samuel e Jonathan.

- Executar testes unitários de todos os módulos.
- Criar testes de integração com roteiros completos.
- Conferir que o mesmo roteiro produz os mesmos resultados na CLI e na simulação.
- Verificar todos os requisitos de Next Fit, IDs, falhas, coalescência e desfragmentação.
- Testar o retorno de passos após alocação, liberação, falha e desfragmentação.
- Corrigir problemas encontrados e remover duplicações entre CLI e GUI.

### Commit 13 — `docs: documenta arquitetura e execução`

**Responsáveis:** Samuel e Jonathan.

- Atualizar o README com objetivo, funcionalidades, dependências e instruções de compilação.
- Documentar como compilar e executar a CLI.
- Documentar como iniciar a GUI e carregar um roteiro.
- Documentar o formato dos roteiros e incluir exemplos em `scripts/`.
- Criar `docs/arquitetura.md`, explicando módulos, fluxo de execução e decisões de design.
- Explicar Strategy, Factory, Memento e a decisão sobre Command.
- Atualizar a descrição e os tópicos do repositório para refletir C++23 e o estado real do projeto.

---

## 6. Regras de comportamento que precisam virar testes

Usar esta lista como checklist de validação. Não considerar uma funcionalidade concluída apenas porque compila.

### Inicialização e alocação

- [ ] `init` cria um único bloco livre e reinicia ID e ponteiro Next Fit.
- [ ] First Fit escolhe o primeiro buraco suficiente em ordem de endereço.
- [ ] Best Fit escolhe o menor buraco suficiente.
- [ ] Worst Fit escolhe o maior buraco suficiente.
- [ ] Empates em First/Best/Worst Fit escolhem o menor endereço.
- [ ] Next Fit começa em zero, procura a partir do ponteiro, considera inteiro o buraco que contém o ponteiro e faz wrap para o início.
- [ ] A alocação acontece no início do buraco escolhido.
- [ ] O espaço restante permanece como buraco.
- [ ] IDs são sequenciais e não são reaproveitados.
- [ ] Falha de alocação não consome ID nem altera o ponteiro Next Fit.
- [ ] A mensagem de falha informa tamanho solicitado, memória livre total e maior buraco.

### Liberação e estatísticas

- [ ] `free_id` e `free_addr` validam os argumentos.
- [ ] Buracos adjacentes são unidos após liberação.
- [ ] `stats` informa memória ocupada, livre, quantidade de buracos, maior buraco e percentual de uso.
- [ ] `show` representa corretamente blocos alocados e livres.

### Desfragmentação

- [ ] INIT posiciona o espaço livre no início.
- [ ] END posiciona o espaço livre no fim.
- [ ] OPT escolhe a posição que minimiza o total de bytes dos blocos movidos.
- [ ] OPT resolve empate escolhendo o maior `k`.
- [ ] A ordem relativa e os IDs dos blocos são preservados.
- [ ] A contagem de blocos e bytes movidos considera somente blocos cujo endereço inicial mudou.
- [ ] O ponteiro Next Fit volta a zero depois da desfragmentação.

### Roteiro e simulação

- [ ] Linhas vazias e comentários são ignorados.
- [ ] `show` e `stats` não viram passos do roteiro visual.
- [ ] Cada comando restante representa um passo, inclusive comandos que falham.
- [ ] Voltar restaura exatamente o estado anterior, incluindo próximo ID e ponteiro Next Fit.
- [ ] Avançar/voltar manualmente pausa a reprodução automática.
- [ ] Reiniciar retorna ao início do roteiro.
- [ ] A GUI apresenta o comando atual, progresso, alterações e erros.

---

## 7. Estratégia de trabalho em dupla

Para reduzir conflitos e evitar que um integrante fique bloqueado pelo outro:

1. **Não implementar contra assinaturas ainda indefinidas.** Os contratos do Commit 6 devem ser revisados pelos dois antes de abrir as tarefas paralelas.
2. **Usar branches separadas por tarefa**, por exemplo `feat/core-inicializacao`, `feat/alocadores-strategy`, `feat/io-roteiros` e `feat/app-controles`.
3. **Evitar editar o mesmo `.cpp` em paralelo.** Quando duas tarefas mexerem na mesma classe, separar a implementação por responsabilidade em arquivos distintos ou combinar quem integra o arquivo final.
4. **Manter os headers como contrato compartilhado.** Alterações em assinaturas públicas devem ser comunicadas e integradas antes de continuar a implementação dependente.
5. **Integrar frequentemente.** Cada branch deve compilar e executar seus testes antes do merge.
6. **Fazer revisão cruzada.** Quem implementou uma estratégia revisa os testes do gerenciador; quem implementou o núcleo revisa a integração da CLI e da simulação.
7. **Não duplicar lógica.** CLI e GUI devem chamar os mesmos módulos de execução; algoritmos e desfragmentação devem existir apenas no `core`/`alocadores`.

### Visão resumida das responsabilidades

| Etapa | Samuel | Jonathan | Paralelismo |
|---|---|---|---|
| Fundação | CMake raiz e estrutura do `core` | CMake dos demais módulos e contratos dos módulos de aplicação | Em conjunto, com integração no mesmo commit |
| Implementação inicial | `init`, `stats`, `show` e invariantes | Strategy, quatro algoritmos, Factory e testes dos algoritmos | Sim |
| Operações principais | `alloc` e integração com Strategy | `free_id`, `free_addr` e coalescência | Sim, em fontes separadas |
| Recursos complementares | Desfragmentação e testes | Parser e leitura de roteiros | Sim |
| Execução | Histórico Memento e simulador passo a passo | Executor compartilhado e CLI | Sim, após definir o contrato do executor |
| Interface gráfica | Mapa, blocos, escala e estatísticas | Controles, arquivos, progresso e reprodução automática | Sim, em componentes separados |
| Finalização | Testes de simulação e revisão | Testes de CLI/parser e revisão | Em conjunto |
| Documentação | Arquitetura e padrões | README, compilação, execução e roteiros | Em conjunto |

A divisão é uma proposta inicial de equilíbrio, não uma atribuição rígida. Se uma etapa ficar mais complexa que o previsto, redistribuir tarefas antes de começar a implementação para evitar que um dos dois acumule as partes mais críticas.

---

## 8. Critério de conclusão do trabalho

O projeto estará pronto quando:

- A CLI e a GUI utilizarem o mesmo núcleo de gerenciamento e os mesmos algoritmos.
- Todos os requisitos funcionais e casos de teste da seção 6 estiverem atendidos.
- A simulação permitir avançar, voltar, reproduzir, pausar e reiniciar, restaurando o estado completo.
- O projeto puder ser configurado e compilado a partir de instruções documentadas.
- Os roteiros de exemplo funcionarem nas interfaces previstas.
- A documentação explicar a arquitetura, a divisão dos módulos e os padrões utilizados.
