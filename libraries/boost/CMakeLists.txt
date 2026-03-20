# Aggregate Boost headers from individual boostorg submodules into the
# build-tree sysroot so that WASM contract code can #include <boost/...>.

set(_boost_libs
    assert
    config
    container
    container_hash
    core
    describe
    detail
    function_types
    fusion
    hana
    intrusive
    io
    move
    mp11
    mpl
    pfr
    predef
    preprocessor
    static_assert
    throw_exception
    tuple
    type_traits
    typeof
    utility
)

foreach(_lib IN LISTS _boost_libs)
    set(_inc_dir "${CMAKE_CURRENT_SOURCE_DIR}/libs/${_lib}/include/boost")
    if(IS_DIRECTORY "${_inc_dir}")
        file(COPY "${_inc_dir}/" DESTINATION "${BASE_BINARY_DIR}/include/boost"
             FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" PATTERN "*.ipp")
    else()
        message(WARNING "Boost submodule '${_lib}' has no include/boost directory")
    endif()
endforeach()
