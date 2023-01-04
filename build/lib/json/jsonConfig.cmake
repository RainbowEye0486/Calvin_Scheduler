include(FindPackageHandleStandardArgs)
set(${CMAKE_FIND_PACKAGE_NAME}_CONFIG ${CMAKE_CURRENT_LIST_FILE})
find_package_handle_standard_args(json CONFIG_MODE)

if(NOT TARGET json::json)
    include("${CMAKE_CURRENT_LIST_DIR}/jsonTargets.cmake")
    if((NOT TARGET json) AND
       (NOT json_FIND_VERSION OR
        json_FIND_VERSION VERSION_LESS 3.2.0))
        add_library(json INTERFACE IMPORTED)
        set_target_properties(json PROPERTIES
            INTERFACE_LINK_LIBRARIES json::json
        )
    endif()
endif()
