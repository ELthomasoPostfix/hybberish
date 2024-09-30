#!/bin/bash

# The file path of the generated log and formatted log.
LOG_FILE_PATH="output/logs/pipeline_test_log.md"
LOG_DIR_PATH=$(dirname $LOG_FILE_PATH)

if [ ! -d $LOG_DIR_PATH ]; then
   echo "The (relative) log directory was not found: ${LOG_DIR_PATH}";
   exit 1;
fi

# Run tests and store its stdout manually.
meson test -C builddir/ 'test the full taylor model flowpipe overapprox pipeline' -v > $LOG_FILE_PATH

# Format the generated logs.
./output/pandoc/format.sh $LOG_FILE_PATH
