set(SPECTRE_LOGIC_SOURCES
    Formula.cpp
    Signature.cpp
    Sort.cpp
    Term.cpp
    Theory.cpp
    Problem.cpp
)
set(SPECTRE_LOGIC_HEADERS
    Formula.hpp
    Signature.hpp
    Sort.hpp
    Term.hpp
    Theory.hpp
    Problem.hpp
)

add_library(logic ${SPECTRE_LOGIC_SOURCES} ${SPECTRE_LOGIC_HEADERS})
target_include_directories(logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(logic util)
