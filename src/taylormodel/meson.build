# Library: Taylor models and Taylor model flowpipes
taylormodel_lib = library('taylormodel', files(
                            'taylormodel.c',
                            'tm_arithmetic.c',
                            'tmflowpipe.c',
                          ),
                          link_with : [logging_lib, utils_lib, fun_lib, varmath_lib, sysode_lib],
                          include_directories : [logging_inc, utils_inc, fun_inc, varmath_inc, sysode_inc],
                          # IMPORTANT: math functions (floor, ceil, ...)
                          # may require explicit linkage to the C math
                          # library via the '-lm' gcc flag
                          link_args : ['-lm'],
)
