#pragma once

#include <cstddef>

struct EstatisticasMemoria {
    std::size_t memoria_ocupada{0};
    std::size_t memoria_livre{0};
    std::size_t quantidade_buracos{0};
    std::size_t maior_buraco{0};

    double percentual_uso{0.0};
};
