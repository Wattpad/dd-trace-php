#include "handlers_pdo.h"

#include <php.h>

#include "compatibility.h"
#include "ddtrace_string.h"
#include "engine_api.h"

typedef void (*ddtrace_zif_handler)(INTERNAL_FUNCTION_PARAMETERS);

struct ddtrace_internal_replacement_handler {
    ddtrace_string name;
    ddtrace_zif_handler new_handler;
    ddtrace_zif_handler *old_handler;
};
typedef struct ddtrace_internal_replacement_handler ddtrace_internal_replacement_handler;

#define DDTRACE_STRING_LITERAL(method) \
    (ddtrace_string) { .ptr = method, .len = sizeof(method) - 1, }

static PHP_FUNCTION(ddtrace_pdo___construct);
static PHP_FUNCTION(ddtrace_pdo_commit);
static PHP_FUNCTION(ddtrace_pdo_exec);
static PHP_FUNCTION(ddtrace_pdo_prepare);
static PHP_FUNCTION(ddtrace_pdo_query);

static ddtrace_zif_handler ddtrace_pdo___construct_handler;
static ddtrace_zif_handler ddtrace_pdo_commit_handler;
static ddtrace_zif_handler ddtrace_pdo_exec_handler;
static ddtrace_zif_handler ddtrace_pdo_prepare_handler;
static ddtrace_zif_handler ddtrace_pdo_query_handler;

static ddtrace_internal_replacement_handler ddtrace_pdo_replacements[] = {
    {
        .name = DDTRACE_STRING_LITERAL("__construct"),
        .new_handler = PHP_FN(ddtrace_pdo___construct),
        .old_handler = &ddtrace_pdo___construct_handler,
    },
    {
        .name = DDTRACE_STRING_LITERAL("commit"),
        .new_handler = PHP_FN(ddtrace_pdo_commit),
        .old_handler = &ddtrace_pdo_commit_handler,
    },
    {
        .name = DDTRACE_STRING_LITERAL("exec"),
        .new_handler = PHP_FN(ddtrace_pdo_exec),
        .old_handler = &ddtrace_pdo_exec_handler,
    },
    {
        .name = DDTRACE_STRING_LITERAL("prepare"),
        .new_handler = PHP_FN(ddtrace_pdo_prepare),
        .old_handler = &ddtrace_pdo_prepare_handler,
    },
    {
        .name = DDTRACE_STRING_LITERAL("query"),
        .new_handler = PHP_FN(ddtrace_pdo_query),
        .old_handler = &ddtrace_pdo_query_handler,
    },
};

void ddtrace_pdo_handlers_startup(void) {
    zend_string *pdo = zend_string_init(ZEND_STRL("pdo"), 0);
    bool pdo_loaded = zend_hash_exists(&module_registry, pdo);
    if (!pdo_loaded) {
        goto pdo_cleanup;
    }

    zval *zv = zend_hash_find(CG(class_table), pdo);
    if (!zv) {
        goto pdo_cleanup;
    }

    zend_class_entry *pdo_ce = Z_PTR_P(zv);
    if (!pdo_ce) {
        goto pdo_cleanup;
    }

    size_t pdo_methods_len = sizeof ddtrace_pdo_replacements / sizeof ddtrace_pdo_replacements[0];
    HashTable *function_table = &pdo_ce->function_table;
    if (!function_table) {
        goto pdo_cleanup;
    }

    for (size_t i = 0; i < pdo_methods_len; ++i) {
        ddtrace_internal_replacement_handler replacement_handler = ddtrace_pdo_replacements[i];
        zend_function *method =
            zend_hash_str_find_ptr(function_table, replacement_handler.name.ptr, replacement_handler.name.len);
        if (method) {
            *replacement_handler.old_handler = method->internal_function.handler;
            method->internal_function.handler = replacement_handler.new_handler;
        }
    }

pdo_cleanup:
    zend_string_release(pdo);
}

void ddtrace_pdo_handlers_shutdown(void) {}

void ddtrace_replacement_handler_dispatch(ddtrace_zif_handler old_handler, INTERNAL_FUNCTION_PARAMETERS) {
    old_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

static PHP_FUNCTION(ddtrace_pdo___construct) {
    ddtrace_replacement_handler_dispatch(ddtrace_pdo___construct_handler, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

static PHP_FUNCTION(ddtrace_pdo_commit) {
    ddtrace_replacement_handler_dispatch(ddtrace_pdo_commit_handler, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

static PHP_FUNCTION(ddtrace_pdo_exec) {
    ddtrace_replacement_handler_dispatch(ddtrace_pdo_exec_handler, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

static PHP_FUNCTION(ddtrace_pdo_prepare) {
    ddtrace_replacement_handler_dispatch(ddtrace_pdo_prepare_handler, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

static PHP_FUNCTION(ddtrace_pdo_query) {
    ddtrace_replacement_handler_dispatch(ddtrace_pdo_query_handler, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
