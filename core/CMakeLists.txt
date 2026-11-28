add_library(core STATIC
  src/gerenciador_memoria.cpp
)

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)