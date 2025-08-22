include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra)
set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)

if (OUTPOST_COVERAGE)
    add_compile_definitions(OUTPOST_USE_ASSERT=0)

    list(APPEND CMAKE_C_FLAGS "-fno-default-inline -fno-inline-functions -fno-inline --coverage")
    list(APPEND CMAKE_CXX_FLAGS "-fno-default-inline -fno-inline-functions -fno-inline --coverage")
    list(APPEND CMAKE_EXE_LINKER_FLAGS "--coverage")
else()
    add_compile_definitions(OUTPOST_USE_ASSERT=1)
endif()

set(OUTPOST_TESTS "")

macro(outpost_add_test)
    set(prefix OUTPOST)
    set(flags)
    set(singleValues MODULE WORKING_DIRECTORY)
    set(multiValues SOURCES DEPENDENCIES INCLUDES)

    if (BUILD_TESTING)
        cmake_parse_arguments(${prefix}
            "${flags}"
            "${singleValues}"
            "${multiValues}"
            ${ARGN}
        )

        file(RELATIVE_PATH module_folder
            "${CMAKE_CURRENT_LIST_DIR}/../.."
            "${CMAKE_CURRENT_LIST_DIR}/.."
        )

        # if the selected RTOS is UNITTEST_RTOS an implementation must be
        # choosen, if non was selected outpost_rtos_unittest_posix will be
        # added to the dependency list
        set(extraDep "")
        if (OUTPOST_OS STREQUAL "UNITTEST_RTOS")
            set(hasImplementation 0)
            foreach(dep ${OUTPOST_DEPENDENCIES})
                if (${dep} MATCHES "outpost_rtos_unittest_*")
                    set(hasImplementation 1)
                endif()
            endforeach()
            
            if (${hasImplementation} EQUAL 0)
                set(extraDep outpost_rtos_unittest_posix)
            endif()
        endif()

        add_executable(${OUTPOST_MODULE}-test ${OUTPOST_SOURCES})
        target_link_libraries(${OUTPOST_MODULE}-test ${OUTPOST_DEPENDENCIES} ${extraDep})

        add_test(NAME ${OUTPOST_MODULE}
            COMMAND ${OUTPOST_MODULE}-test
            WORKING_DIRECTORY ${OUTPOST_WORKING_DIRECTORY}
        )

        set_target_properties(${OUTPOST_MODULE}-test
            PROPERTIES EXPORT_COMPILE_COMMANDS ON
        )

        add_custom_target(${OUTPOST_MODULE}-run
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${OUTPOST_MODULE}-test
            COMMENT "Run test for '${OUTPOST_MODULE}'"
        )

        add_dependencies(${OUTPOST_MODULE}-run ${OUTPOST_MODULE}-test)

        add_custom_target(${OUTPOST_MODULE}-coverage
            COMMAND find ${CMAKE_CURRENT_BINARY_DIR} -name "*.gcda" -delete
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${OUTPOST_MODULE}-test
            COMMAND gcovr
                --decisions
                # --exclude-unreachable-branches
                # --gcov-ignore-parse-errors=negative_hits.warn
                --root=${CMAKE_SOURCE_DIR}/modules
                --filter=${module_folder}/src/
                --object-directory=${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}
                -e "/usr/*"
                -e "test/*"
                -e "default/*"
                -e "tools/*"
                -e "contrib/*"
                --xml-pretty
                -o ${CMAKE_BINARY_DIR}/${OUTPOST_MODULE}.xml
                --json ${CMAKE_BINARY_DIR}/${OUTPOST_MODULE}_coverage.json
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/modules
            VERBATIM
            DEPENDS ${OUTPOST_MODULE}-test
            COMMENT "Run test and gcovr for '${OUTPOST_MODULE}'"
        )

        add_custom_target(${OUTPOST_MODULE}-valgrind
            COMMAND mkdir -p ${CMAKE_BINARY_DIR}/${OUTPOST_MODULE}
            COMMAND valgrind
                --track-origins=yes
                --leak-check=full
                --fair-sched=yes
                --log-file=${CMAKE_BINARY_DIR}/${OUTPOST_MODULE}/valgrind.log
                ${CMAKE_CURRENT_BINARY_DIR}/${OUTPOST_MODULE}-test
            VERBATIM
            DEPENDS ${OUTPOST_MODULE}-test
            COMMENT "Run test with valgrind for '${OUTPOST_MODULE}'"
        )

        add_dependencies(${OUTPOST_MODULE}-coverage ${OUTPOST_MODULE}-test)
        set(OUTPOST_TESTS "${OUTPOST_TESTS}${OUTPOST_MODULE};" PARENT_SCOPE)
    endif()
endmacro()

macro(outpost_generate_test)
    add_custom_target(run-unittests
        COMMAND;
        # this will fail if the list is empty
        DEPENDS "$<JOIN:${OUTPOST_TESTS},-run;>-run;"
    )
    add_custom_target(run-unittests-valgrind
        COMMAND;
        # this will fail if the list is empty
        DEPENDS "$<JOIN:${OUTPOST_TESTS},-valgrind;>-valgrind;"
    )
    if (OUTPOST_COVERAGE)
        add_custom_target(run-unittests-coverage
            COMMAND mkdir -p ${CMAKE_BINARY_DIR}/report
            COMMAND gcovr
                --add-tracefile "${CMAKE_BINARY_DIR}/*_coverage.json"
                --root=${CMAKE_SOURCE_DIR}/modules
                --xml-pretty
                --exclude-unreachable-branches
                --print-summary
                --decisions
                -o "${CMAKE_CURRENT_BINARY_DIR}/coverage.xml"
                --html-details ${CMAKE_CURRENT_BINARY_DIR}/report/index.html > "${CMAKE_CURRENT_BINARY_DIR}/summary.txt"
            # this will fail if the list is empty
            DEPENDS "$<JOIN:${OUTPOST_TESTS},-coverage;>-coverage;"
            VERBATIM
            COMMENT "Create coverage report"
        )
    endif()
endmacro()
