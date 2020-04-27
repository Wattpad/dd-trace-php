#include "configuration.h"

#include <stdlib.h>

#include "ddtrace_string.h"
#include "env_config.h"

struct ddtrace_memoized_configuration_t ddtrace_memoized_configuration = {
#define CHAR(...) NULL, FALSE,
#define BOOL(...) FALSE, FALSE,
#define INT(...) 0, FALSE,
#define DOUBLE(...) 0.0, FALSE,
    DD_CONFIGURATION
#undef CHAR
#undef BOOL
#undef INT
#undef DOUBLE
        PTHREAD_MUTEX_INITIALIZER};

void ddtrace_reload_config(TSRMLS_D) {
#define CHAR(getter_name, ...)                            \
    if (ddtrace_memoized_configuration.getter_name) {     \
        free(ddtrace_memoized_configuration.getter_name); \
    }                                                     \
    ddtrace_memoized_configuration.__is_set_##getter_name = FALSE;
#define BOOL(getter_name, ...) ddtrace_memoized_configuration.__is_set_##getter_name = FALSE;
#define INT(getter_name, ...) ddtrace_memoized_configuration.__is_set_##getter_name = FALSE;
#define DOUBLE(getter_name, ...) ddtrace_memoized_configuration.__is_set_##getter_name = FALSE;

    DD_CONFIGURATION

#undef CHAR
#undef BOOL
#undef INT
#undef DOUBLE
    // repopulate config
    ddtrace_initialize_config(TSRMLS_C);
}

void ddtrace_initialize_config(TSRMLS_D) {
    // read all values to memoize them

    // CHAR returns a copy of a value that we need to free
#define CHAR(getter_name, env_name, default, ...)                                  \
    do {                                                                           \
        pthread_mutex_lock(&ddtrace_memoized_configuration.mutex);                 \
        ddtrace_memoized_configuration.getter_name =                               \
            ddtrace_get_c_string_config_with_default(env_name, default TSRMLS_CC); \
        ddtrace_memoized_configuration.__is_set_##getter_name = TRUE;              \
        pthread_mutex_unlock(&ddtrace_memoized_configuration.mutex);               \
    } while (0);
#define BOOL(getter_name, env_name, default, ...)                                                          \
    do {                                                                                                   \
        pthread_mutex_lock(&ddtrace_memoized_configuration.mutex);                                         \
        ddtrace_memoized_configuration.getter_name = ddtrace_get_bool_config(env_name, default TSRMLS_CC); \
        ddtrace_memoized_configuration.__is_set_##getter_name = TRUE;                                      \
        pthread_mutex_unlock(&ddtrace_memoized_configuration.mutex);                                       \
    } while (0);
#define INT(getter_name, env_name, default, ...)                                                          \
    do {                                                                                                  \
        pthread_mutex_lock(&ddtrace_memoized_configuration.mutex);                                        \
        ddtrace_memoized_configuration.getter_name = ddtrace_get_int_config(env_name, default TSRMLS_CC); \
        ddtrace_memoized_configuration.__is_set_##getter_name = TRUE;                                     \
        pthread_mutex_unlock(&ddtrace_memoized_configuration.mutex);                                      \
    } while (0);
#define DOUBLE(getter_name, env_name, default, ...)                                                          \
    do {                                                                                                     \
        pthread_mutex_lock(&ddtrace_memoized_configuration.mutex);                                           \
        ddtrace_memoized_configuration.getter_name = ddtrace_get_double_config(env_name, default TSRMLS_CC); \
        ddtrace_memoized_configuration.__is_set_##getter_name = TRUE;                                        \
        pthread_mutex_unlock(&ddtrace_memoized_configuration.mutex);                                         \
    } while (0);

    DD_CONFIGURATION

#undef CHAR
#undef BOOL
#undef INT
#undef DOUBLE
}

void ddtrace_config_shutdown(void) {
    // read all values to memoize them

    // CHAR returns a copy of a value that we need to free
#define CHAR(getter_name, env_name, default, ...)                      \
    do {                                                               \
        pthread_mutex_lock(&ddtrace_memoized_configuration.mutex);     \
        if (ddtrace_memoized_configuration.getter_name) {              \
            free(ddtrace_memoized_configuration.getter_name);          \
            ddtrace_memoized_configuration.getter_name = NULL;         \
        }                                                              \
        ddtrace_memoized_configuration.__is_set_##getter_name = FALSE; \
        pthread_mutex_unlock(&ddtrace_memoized_configuration.mutex);   \
    } while (0);
#define BOOL(getter_name, env_name, default, ...)                      \
    do {                                                               \
        pthread_mutex_lock(&ddtrace_memoized_configuration.mutex);     \
        ddtrace_memoized_configuration.__is_set_##getter_name = FALSE; \
        pthread_mutex_unlock(&ddtrace_memoized_configuration.mutex);   \
    } while (0);
#define INT(getter_name, env_name, default, ...)                       \
    do {                                                               \
        pthread_mutex_lock(&ddtrace_memoized_configuration.mutex);     \
        ddtrace_memoized_configuration.__is_set_##getter_name = FALSE; \
        pthread_mutex_unlock(&ddtrace_memoized_configuration.mutex);   \
    } while (0);
#define DOUBLE(getter_name, env_name, default, ...)                    \
    do {                                                               \
        pthread_mutex_lock(&ddtrace_memoized_configuration.mutex);     \
        ddtrace_memoized_configuration.__is_set_##getter_name = FALSE; \
        pthread_mutex_unlock(&ddtrace_memoized_configuration.mutex);   \
    } while (0);

    DD_CONFIGURATION

#undef CHAR
#undef BOOL
#undef INT
#undef DOUBLE
}

// define configuration getters macros
#define CHAR(getter_name, ...) extern inline char* getter_name(void);
#define BOOL(getter_name, ...) extern inline bool getter_name(void);
#define INT(getter_name, ...) extern inline int64_t getter_name(void);
#define DOUBLE(getter_name, ...) extern inline double getter_name(void);

DD_CONFIGURATION

#undef CHAR
#undef BOOL
#undef INT
#undef DOUBLE

bool ddtrace_config_bool(ddtrace_string subject, bool default_value) {
    ddtrace_string str_1 = {
        .ptr = "1",
        .len = 1,
    };
    ddtrace_string str_true = {
        .ptr = "true",
        .len = sizeof("true") - 1,
    };
    if (ddtrace_string_equals(subject, str_1) || ddtrace_string_equals(subject, str_true)) {
        return true;
    }
    ddtrace_string str_0 = {
        .ptr = "0",
        .len = 1,
    };
    ddtrace_string str_false = {
        .ptr = "false",
        .len = sizeof("false") - 1,
    };
    if (ddtrace_string_equals(subject, str_0) || ddtrace_string_equals(subject, str_false)) {
        return false;
    }
    return default_value;
}

bool ddtrace_config_distributed_tracing_enabled(TSRMLS_D) {
    ddtrace_string distributed_tracing_enabled =
        ddtrace_string_cstring_ctor(ddtrace_getenv(ZEND_STRL("DD_DISTRIBUTED_TRACING") TSRMLS_CC));
    if (distributed_tracing_enabled.len) {
        /* We need to lowercase the str for ddtrace_config_bool.
         * We know it's already been duplicated by ddtrace_getenv, so we can
         * lower it in-place.
         */
        zend_str_tolower(distributed_tracing_enabled.ptr, distributed_tracing_enabled.len);
        bool result = ddtrace_config_bool(distributed_tracing_enabled, true);
        efree(distributed_tracing_enabled.ptr);
        return result;
    }
    if (distributed_tracing_enabled.ptr) {
        efree(distributed_tracing_enabled.ptr);
    }
    return true;
}
