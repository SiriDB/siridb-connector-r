#ifndef RSDBCL_h
#define RSDBCL_h

#include <R.h>
#include <Rdefines.h>
#include <R_ext/Rdynload.h>
#include "libsuv/suv.h"

static uv_loop_t loop;
siridb_t * siridb;
suv_buf_t * buf;

static void connect_cb(siridb_req_t * req);
static void query_cb(siridb_req_t * req);
static void insert_cb(siridb_req_t * req);
static void on_close(void * data, const char * msg);//TODOK
static void on_error(void * data, const char * msg);//TODOK
SEXP sconnect(SEXP env, SEXP x, SEXP f);
SEXP squery(SEXP env, SEXP x, SEXP f);
SEXP sinsert(SEXP env, SEXP x, SEXP f);
SEXP sclose(void);
SEXP print_resp(siridb_resp_t * resp);
SEXP print_timeit(siridb_timeit_t * timeit);//TODOK
SEXP print_select(siridb_select_t * select);
SEXP print_list(siridb_list_t * list);
SEXP print_count(uint64_t count);
SEXP print_calc(uint64_t calc);
SEXP print_show(siridb_show_t * show);

typedef struct {
    SEXP cb;
    SEXP env;
} work_t;

#endif