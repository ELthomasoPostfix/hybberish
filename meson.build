project('hybberish', 'c',
  version : '0.1',
  default_options : ['warning_level=3']
)

# Define the locations of all header files.
utils_inc = include_directories('src/utils')
logging_inc = include_directories('src/logging')
fun_inc = include_directories('src/fun')
sysode_inc = include_directories('src/systems')
varmath_inc = include_directories('src/varmath')
odeparse_inc = include_directories('src/parsers')
taylormodel_inc = include_directories('src/taylormodel')

# A shorthand for all include (header) dependencies of the logging library.
logging_dependencies_inc = [utils_inc, fun_inc, sysode_inc, varmath_inc, taylormodel_inc]
# A shorthand for the logging library and all its include (header) dependencies.
logging_with_deps_inc = [logging_inc, logging_dependencies_inc]

# Calling 'subdir' runs the contained meson.build file,
# building the meson library we defined in each file.
# Do this AFTER all include directories are defined, so that any
# lib could know about any include dir (header files) if needed.
subdir('src/utils')
subdir('src/logging')
subdir('src/fun')
subdir('src/systems')
subdir('src/varmath')
subdir('src/parsers')
subdir('src/taylormodel')

subdir('tests')
