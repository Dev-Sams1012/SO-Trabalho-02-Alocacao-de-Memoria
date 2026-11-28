# Coalescência (união de buracos adjacentes) em 30 bytes.
init 30
alloc 10 first
alloc 10 first
alloc 10 first
# Memória cheia: id=1 @0, id=2 @10, id=3 @20.
freeid 1
# Um buraco @0 +10.
freeaddr 20
# Libera o id=3 pelo endereço: dois buracos separados (@0 +10 e @20 +10).
freeid 2
# O buraco do meio une os dois lados: um único buraco @0 +30.