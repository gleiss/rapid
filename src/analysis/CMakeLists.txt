set(SPECTRE_ANALYSIS_SOURCES
    Semantics.cpp
    TraceLemmas.cpp
)
set(SPECTRE_ANALYSIS_HEADERS
    Semantics.hpp
    TraceLemmas.hpp
)

add_library(analysis ${SPECTRE_ANALYSIS_SOURCES} ${SPECTRE_ANALYSIS_HEADERS})
target_include_directories(analysis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(analysis program logic)
