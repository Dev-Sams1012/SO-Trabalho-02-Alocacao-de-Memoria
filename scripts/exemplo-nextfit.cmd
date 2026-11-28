# exemplo do Next Fit
init 32
alloc 8 next
alloc 8 next
alloc 8 next
freeid 1
alloc 4 next
alloc 6 next
# Esperado: id=4 @24 (First Fit usaria @0) e id=5 @0 (First Fit usaria @24).
# Estado final: id=5 @0 +6B, id=2 @8 +8B, id=3 @16 +8B, id=4 @24 +4B.
# Ocupado 26 B | Livre 6 B | Buracos 2 | Maior buraco 4 | Uso efetivo 81.25%