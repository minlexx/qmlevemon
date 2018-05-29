execute_process(COMMAND readelf --wide --dynamic ${TARGET} ERROR_VARIABLE readelf_errors OUTPUT_VARIABLE out RESULT_VARIABLE result)

if (NOT result EQUAL 0)
    message(FATAL_ERROR "readelf failed on ${TARGET} exit(${result}): ${readelf_errors}")
endif()

string(REPLACE "\n" ";" lines "${out}")
set(extralibs)
foreach(line ${lines})
    string(REGEX MATCH ".*\\(NEEDED\\) +Shared library: +\\[(.+)\\]$" matched ${line})
    set(currentLib ${CMAKE_MATCH_1})

    if(NOT ${currentLib} MATCHES "libQt5.*" AND matched)
        find_file(ourlib-${currentLib} ${currentLib} HINTS ${OUTPUT_DIR} ${EXPORT_DIR} ${ECM_ADDITIONAL_FIND_ROOT_PATH} NO_DEFAULT_PATH PATH_SUFFIXES lib)

        if(ourlib-${currentLib})
            list(APPEND extralibs "${ourlib-${currentLib}}")
        else()
            message(STATUS "could not find ${currentLib} in ${OUTPUT_DIR} ${EXPORT_DIR}/lib/ ${ECM_ADDITIONAL_FIND_ROOT_PATH}")
        endif()
    endif()
endforeach()

if (ANDROID_EXTRA_LIBS)
    foreach (extralib ${ANDROID_EXTRA_LIBS})
        message(STATUS "manually specified extra libray: " ${extralib})
        list(APPEND extralibs ${extralib})
    endforeach()
endif()

if(extralibs)
    string(REPLACE ";" "," libs "${extralibs}")
    set(extralibs "\"android-extra-libs\": \"${libs}\",")
endif()

set(extraplugins)
foreach(folder "share" "lib/qml") #now we check for folders with extra stuff
    set(plugin "${EXPORT_DIR}/${folder}")
    if(EXISTS "${plugin}")
        if(extraplugins)
            set(extraplugins "${extraplugins},${plugin}")
        else()
            set(extraplugins "${plugin}")
        endif()
    endif()
endforeach()
if(extraplugins)
    set(extraplugins "\"android-extra-plugins\": \"${extraplugins}\",")
endif()

file(READ "${INPUT_FILE}" CONTENTS)
file(READ "stl" stl_contents)

string(REPLACE "##EXTRALIBS##" "${extralibs}" NEWCONTENTS "${CONTENTS}")
string(REPLACE "##EXTRAPLUGINS##" "${extraplugins}" NEWCONTENTS "${NEWCONTENTS}")
string(REPLACE "##CMAKE_CXX_STANDARD_LIBRARIES##" "${stl_contents}" NEWCONTENTS "${NEWCONTENTS}")
file(WRITE "${OUTPUT_FILE}" ${NEWCONTENTS})