set(SPECTRE_DECLARATIONS_SOURCES
    SymbolDeclarations.cpp
)
set(SPECTRE_DECLARATIONS_HEADERS
    SymbolDeclarations.hpp
)

add_library(declarations ${SPECTRE_DECLARATIONS_SOURCES} ${SPECTRE_DECLARATIONS_HEADERS})
target_include_directories(declarations PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(declarations logic program)
