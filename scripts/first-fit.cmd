# Comparação dos algoritmos. Prepara 4 buracos (@0 +10, @14 +6, @24 +20,
# @48 +16) e faz um alloc de 5 bytes na última linha.
# Para comparar, troque "first" na ÚLTIMA linha por best, worst ou next:
#   first -> id=7 @0   (primeiro buraco em que cabe)
#   best  -> id=7 @14  (menor buraco em que cabe: +6)
#   worst -> id=7 @24  (maior buraco: +20)
#   next  -> id=7 @48  (a busca começa após o id=6, que termina em 48)
init 64
alloc 10 first
alloc 4 first
alloc 6 first
alloc 4 first
alloc 20 first
alloc 4 first
freeid 1
freeid 3
freeid 5
alloc 5 first