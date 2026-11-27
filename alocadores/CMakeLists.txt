add_library(alocadores STATIC
  src/first_fit.cpp
  src/best_fit.cpp
  src/worst_fit.cpp
  src/next_fit.cpp
  src/fabrica_alocadores.cpp
)

target_include_directories(alocadores PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# alocadores depende do core (usa EstadoMemoria e a interface Strategy).
target_link_libraries(alocadores PUBLIC core)