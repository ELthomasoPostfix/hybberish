# Library: Utility functions without any dependencies
utils_lib = library('utils', files(
                      'utils.c',
                    ),

                    # Explicitly define EMPTY dependencies.
                    # The 'utils' library should always have zero dependencies.
                    link_with : [],
                    include_directories : [],

                    # IMPORTANT: math functions (floor, ceil, ...)
                    # may require explicit linkage to the C math
                    # library via the '-lm' gcc flag
                    link_args : ['-lm'],
)
