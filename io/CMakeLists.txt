add_library(io STATIC
  src/interpretador.cpp
  src/roteiro.cpp
)

target_include_directories(io PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# io reutiliza só os tipos FitAlg e Option do core (não os chama).
target_link_libraries(io PUBLIC core)
