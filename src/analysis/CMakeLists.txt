set(SPECTRE_ANALYSIS_SOURCES
    Semantics.cpp
    SemanticsHelper.cpp
    AnalysisPreComputation.cpp
    TraceLemmas.cpp
    TheoryAxioms.cpp
    ProgramTraverser.cpp
    LemmasVariableValues.cpp
    LemmasIterators.cpp
    LemmasOther.cpp
    LemmasTwoTraces.cpp
)
set(SPECTRE_ANALYSIS_HEADERS
    Semantics.hpp
    SemanticsHelper.hpp
    AnalysisPreComputation.hpp
    TraceLemmas.hpp
    TheoryAxioms.hpp
    ProgramTraverser.hpp
    LemmasVariableValues.hpp
    LemmasIterators.hpp
    LemmasOther.hpp
    LemmasTwoTraces.hpp
)

add_library(analysis ${SPECTRE_ANALYSIS_SOURCES} ${SPECTRE_ANALYSIS_HEADERS})
target_include_directories(analysis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(analysis program logic declarations)
