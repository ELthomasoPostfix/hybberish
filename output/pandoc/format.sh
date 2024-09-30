#!/bin/bash
# Usage:
#      $1 : The path to the unformatted, input log file.

OUTPUT_DIR="output/"  # The dir containing all source code for generating output.
PANDOC_DIR="${OUTPUT_DIR}pandoc/"    # A dir containing "source code"
RESULT_DIR="${OUTPUT_DIR}formatted/" # The output dir; the formatted log storage
LOG_FILE_PATH=$1  # The path to the unformatted, input log.

# The path prefix and one file extension postfix are ignored.
RESULT_FILE_NAME=$(basename -- $1)        # Strip path preceding file name.
RESULT_FILE_NAME="${RESULT_FILE_NAME%.*}" # Strip file extension.

pandoc $LOG_FILE_PATH                                   \
       -s                                               \
       -f markdown-smart                                \
       --wrap=preserve                                  \
       --mathjax                                        \
       --lua-filter="${PANDOC_DIR}filter.lua"           \
       --include-in-header "${PANDOC_DIR}header.html"   \
       --metadata title="Hybberish LOG"                 \
       -o "${RESULT_DIR}${RESULT_FILE_NAME}.html"
