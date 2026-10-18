# Provide an installation-shaped data tree without installing GDL. Directory
# links keep edits to .pro files/resources visible immediately. On platforms
# without symlink privileges, refresh copies whenever GDL is built instead.
set(GDL_BUILD_DATA_DIR "${CMAKE_BINARY_DIR}/share/gnudatalanguage")
file(MAKE_DIRECTORY "${GDL_BUILD_DATA_DIR}")
add_custom_target(gdl_build_resources)
function(gdl_build_resource source name)
  set(destination "${GDL_BUILD_DATA_DIR}/${name}")
  if(NOT EXISTS "${destination}")
    execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
      "${source}" "${destination}" RESULT_VARIABLE result ERROR_QUIET)
  endif()
  if(NOT IS_SYMLINK "${destination}")
    if(CMAKE_VERSION VERSION_LESS 3.26)
      set(copy_command copy_directory)
    else()
      set(copy_command copy_directory_if_different)
    endif()
    add_custom_command(TARGET gdl_build_resources POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E ${copy_command} "${source}" "${destination}"
      VERBATIM)
  endif()
endfunction()
gdl_build_resource("${CMAKE_SOURCE_DIR}/src/pro" lib)
gdl_build_resource("${CMAKE_SOURCE_DIR}/resource" resource)
file(MAKE_DIRECTORY "${GDL_BUILD_DATA_DIR}/dlm")
add_dependencies(gdl gdl_build_resources)

# Shell single-quoting also handles spaces, apostrophes and dollar signs in
# source/build paths. Expand target locations at generation time, per config.
function(gdl_shell_quote output value)
  string(REPLACE "'" "'\"'\"'" value "${value}")
  set(${output} "'${value}'" PARENT_SCOPE)
endfunction()
gdl_shell_quote(GDL_RUN_DATA "${GDL_BUILD_DATA_DIR}")
gdl_shell_quote(GDL_RUN_PLPLOT "${CMAKE_SOURCE_DIR}/src/plplot/data")
gdl_shell_quote(GDL_RUN_DLM "${CMAKE_BINARY_DIR}/src/dlm")
# Target filenames can vary with the configuration and output-directory options.
set(GDL_RUN_BINARY "$<TARGET_FILE:gdl>")
set(GDL_RUN_PATH_SEPARATOR ":")
if(WIN32 AND NOT CYGWIN)
  set(GDL_RUN_PATH_SEPARATOR ";")
endif()

set(GDL_BUILD_LAUNCHER "${CMAKE_BINARY_DIR}/run-gdl")
if(CMAKE_CONFIGURATION_TYPES)
  set(GDL_BUILD_LAUNCHER "${CMAKE_BINARY_DIR}/$<CONFIG>/run-gdl")
  list(GET CMAKE_CONFIGURATION_TYPES 0 GDL_RUN_DEFAULT_CONFIG)
  if("Release" IN_LIST CMAKE_CONFIGURATION_TYPES)
    set(GDL_RUN_DEFAULT_CONFIG Release)
  endif()
  gdl_shell_quote(GDL_RUN_BUILD_DIR "${CMAKE_BINARY_DIR}")
  configure_file("${CMAKE_SOURCE_DIR}/scripts/run-gdl-config.in"
    "${CMAKE_BINARY_DIR}/run-gdl" @ONLY NEWLINE_STYLE UNIX)
endif()
# The launchers are shell scripts. Without NEWLINE_STYLE, CMake writes text-mode
# (CRLF) files on Windows, which sh cannot run.
configure_file("${CMAKE_SOURCE_DIR}/scripts/run-gdl.in"
  "${CMAKE_BINARY_DIR}/CMakeFiles/run-gdl.in" @ONLY NEWLINE_STYLE UNIX)
set(gdl_launcher_newline)
if(NOT CMAKE_VERSION VERSION_LESS 3.20)
  set(gdl_launcher_newline NEWLINE_STYLE UNIX)
elseif(WIN32)
  message(WARNING "CMake 3.20 or newer is needed to generate a usable run-gdl on Windows.")
endif()
# file(GENERATE) preserves the executable permissions of its input file.
file(GENERATE OUTPUT "${GDL_BUILD_LAUNCHER}"
  INPUT "${CMAKE_BINARY_DIR}/CMakeFiles/run-gdl.in" ${gdl_launcher_newline})
