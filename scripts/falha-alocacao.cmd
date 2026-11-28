# Falhas de alocação e de liberação: nenhuma altera a memória nem consome id.
init 20
alloc 6 first
alloc 6 first
alloc 6 first
freeid 1
# Buracos: @0 +6 e @18 +2 (livre total 8 B, maior buraco 6 B).
alloc 7 first
# Esperado: Erro: sem espaço contíguo para 7 bytes (livre total: 8, maior buraco: 6)
# Há 8 B livres, mas não contíguos: fragmentação externa.
freeid 7
# Esperado: erro, id inexistente.
freeaddr 3
# Esperado: erro, 3 não é o início de um bloco alocado.
alloc 2 first
# Esperado: id=4 @0 (os pedidos que falharam não gastaram ids).