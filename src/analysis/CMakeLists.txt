set(SPECTRE_ANALYSIS_SOURCES
    Semantics.cpp
    SemanticsHelper.cpp
    AnalysisPreComputation.cpp
    TraceLemmas.cpp
    StaticAnalysis.cpp
    TheoryAxioms.cpp
    ProgramTraverser.cpp
)
set(SPECTRE_ANALYSIS_HEADERS
    Semantics.hpp
    SemanticsHelper.hpp
    AnalysisPreComputation.hpp
    TraceLemmas.hpp
    StaticAnalysis.hpp
    TheoryAxioms.hpp
    ProgramTraverser.hpp
)

add_library(analysis ${SPECTRE_ANALYSIS_SOURCES} ${SPECTRE_ANALYSIS_HEADERS})
target_include_directories(analysis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(analysis program logic declarations)
