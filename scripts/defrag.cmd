# Desfragmentação, com o exemplo do enunciado (memória de 64 bytes).
# Cada trecho recomeça com "init 64", que reinicia a simulação.
# Antes de cada defrag: id=1 @0 +10B e id=3 @18 +6B (buraco @10 +8B).

init 64
alloc 10 first
alloc 8 first
alloc 6 first
freeid 2
defrag init
# Esperado: 2 blocos movidos, 16 B. id=1 @48 +10B, id=3 @58 +6B.

init 64
alloc 10 first
alloc 8 first
alloc 6 first
freeid 2
defrag end
# Esperado: 1 bloco movido, 6 B. id=1 @0 +10B, id=3 @10 +6B.

init 64
alloc 10 first
alloc 8 first
alloc 6 first
freeid 2
defrag opt
# Esperado: 1 bloco movido, 6 B. id=1 @0 +10B, id=3 @10 +6B (k=2, empata com k=1).