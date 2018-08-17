set(SPECTRE_ANALYSIS_SOURCES
    Properties.cpp
    PropertiesTime.cpp
    Analyzer.cpp
    Semantics.cpp
)
set(SPECTRE_ANALYSIS_HEADERS
    Properties.hpp
    PropertiesTime.hpp
    Analyzer.hpp
    Semantics.hpp
)

add_library(analysis ${SPECTRE_ANALYSIS_SOURCES} ${SPECTRE_ANALYSIS_HEADERS})
target_include_directories(analysis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(analysis program logic)
