#include "logging.h"

/**
 * @brief The defintion of the global logging level.
 * @details A definition within the logging library is needed, because
 * else all other libraries that wanted to include logging would need
 * to specify their own definition.
 */
LogLevel logLevelHybberish = LOG_NONE;
