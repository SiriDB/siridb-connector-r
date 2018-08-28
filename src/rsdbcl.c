#include "rsdbcl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


SEXP sconnect(SEXP env, SEXP x, SEXP f)
{
    work_t * w = (work_t *) malloc(sizeof(work_t));
    w->cb = f;
    w->env = env;

    const char * user = CHAR(asChar(VECTOR_ELT(x, 0)));
    const char * passwd = CHAR(asChar(VECTOR_ELT(x, 1)));
    const char * dbname = CHAR(asChar(VECTOR_ELT(x, 2)));
    const char * server = CHAR(asChar(VECTOR_ELT(x, 3)));
    int port = asInteger(VECTOR_ELT(x, 4));

    struct sockaddr_in addr;

    uv_loop_init(&loop);
    uv_ip4_addr(server, port, &addr);

    siridb = siridb_create();
    buf = suv_buf_create(siridb);
    buf->onclose = on_close;
    buf->onerror = on_error;

    siridb_req_t * req = siridb_req_create(siridb, connect_cb, NULL);
    suv_connect_t * connect = suv_connect_create(req, user, passwd, dbname);
    connect->data = w;
    req->data = (void *) connect;

    suv_connect(&loop, connect, buf, (struct sockaddr *) &addr);
    uv_run(&loop, UV_RUN_DEFAULT);

    return R_NilValue;
}

SEXP squery(SEXP env, SEXP x, SEXP f)
{
    work_t * w = (work_t *) malloc(sizeof(work_t));
    w->cb = f;
    w->env = env;

    const char * query = CHAR(asChar(VECTOR_ELT(x, 0)));

    siridb_req_t * req = siridb_req_create(siridb, query_cb, NULL);
    suv_query_t * suvquery = suv_query_create(req, query);
    suvquery->data = w;
    req->data = (void *) suvquery;

    suv_query(suvquery);

    return R_NilValue;
}

SEXP sinsert(SEXP env, SEXP x, SEXP f)
{
    work_t * w = (work_t *) malloc(sizeof(work_t));
    w->cb = f;
    w->env = env;

    SEXP s, pts, tv, val;
    const char * name = CHAR(asChar(VECTOR_ELT(VECTOR_ELT(x, 0), 0)));
    const char * typ = CHAR(asChar(VECTOR_ELT(VECTOR_ELT(x, 0), 1)));
    int n;
    int m = length(x);
    siridb_series_tp tp;

    siridb_series_t * iseries[m];
    siridb_point_t * ipoint;


    for (size_t i = 0; i < m; i++)
    {
        s = VECTOR_ELT(x, i);
        name = CHAR(asChar(VECTOR_ELT(s, 0)));
        typ = CHAR(asChar(VECTOR_ELT(s, 1)));
        pts = VECTOR_ELT(s, 2);
        n = length(pts);

        if (!strcmp(typ, "integer"))
        {
            tp = SIRIDB_SERIES_TP_INT64;
        }
        else if (!strcmp(typ, "float"))
        {
            tp = SIRIDB_SERIES_TP_REAL;
        }
        else if (!strcmp(typ, "string"))
        {
            tp = SIRIDB_SERIES_TP_STR;
        }
        else
        {
            // Rcpp::stop("incorrect datatype");
        }

        iseries[i] = siridb_series_create(
            tp,
            name,
            n);

        for (size_t j = 0; j < n; j++) {
            tv = VECTOR_ELT(pts, j);
            val = VECTOR_ELT(tv, 1);
            if (TYPEOF())
            ipoint = iseries[i]->points + j;
            ipoint->ts = (uint64_t) asInteger(VECTOR_ELT(tv, 0));

            switch (TYPEOF(x)
            {
                case LGLSXP:
                    ipoint->via.int64 = (int64_t) asInteger(VECTOR_ELT(tv, 1));
                    break;
                case INTSXP:
                    ipoint->via.int64 = (int64_t) asInteger(VECTOR_ELT(tv, 1));
                    break;
                case REALSXP:
                    ipoint->via.real = (double_t) asReal(VECTOR_ELT(tv, 1));
                    break;
                case STRSXP:
                    ipoint->via.str = (char *) strdup(CHAR(asChar(VECTOR_ELT(tv, 1))));
                    break;

            }
        }
    }

    siridb_req_t * req = siridb_req_create(siridb, insert_cb, NULL);
    suv_insert_t * suvinsert = suv_insert_create(req, iseries, m);

    suvinsert->data = w;
    req->data = (void *) suvinsert;

    suv_insert(suvinsert);

    for (size_t i = 0; i < m; i++)
    {
        siridb_series_destroy(iseries[i]);
    }

    return R_NilValue;
}

SEXP sclose(void)
{
    suv_close(buf, NULL);
    return R_NilValue;
}

static void connect_cb(siridb_req_t * req)
{
    char msg[200];
    SEXP out;
    SEXP status = ScalarInteger(req->status);
    suv_connect_t * connect = (suv_connect_t *) req->data;
    work_t * w = (work_t *) connect->data;

    if (req->status)
    {
        sprintf(msg, "connect failed: %s", suv_strerror(req->status));
        out = mkString(msg);
    }
    else
    {
        switch (req->pkg->tp)
        {
        case CprotoResAuthSuccess:
            out = R_NilValue;
            break;
        case CprotoErrAuthCredentials:
            out = mkString("auth failed: invalid credentials");
            break;
        case CprotoErrAuthUnknownDb:
            out = mkString("auth failed: unknown database");
            break;
        default:
            sprintf(msg, "auth failed: unknown error (%u)", req->pkg->tp);
            out = mkString(msg);
        }
    }

    SEXP call = PROTECT(LCONS(w->cb, LCONS(out, LCONS(status, R_NilValue))));
    R_forceAndCall(call, 2, w->env);
    UNPROTECT(2);

    /* destroy suv_connect_t */
    suv_connect_destroy(connect);

    /* destroy siridb request */
    siridb_req_destroy(req);

    return;
}

static void query_cb(siridb_req_t * req)
{
    char msg[200];
    SEXP out;
    SEXP status = ScalarInteger(req->status);

    suv_query_t * connect = (suv_query_t *) req->data;
    work_t * w = (work_t *) connect->data;

    if (req->status != 0)
    {
        sprintf(msg, "error handling request: %s", suv_strerror(req->status));
        out = mkString(msg);
    }
    else
    {
        siridb_resp_t * resp = siridb_resp_create(req->pkg, NULL);
        out = print_resp(resp);
        siridb_resp_destroy(resp);
    }

    SEXP call = PROTECT(LCONS(w->cb, LCONS(out, LCONS(status, R_NilValue))));
    R_forceAndCall(call, 2, w->env);
    UNPROTECT(2);


    suv_query_destroy((suv_query_t *) req->data);
    siridb_req_destroy(req);
}

static void insert_cb(siridb_req_t * req)
{
    char msg[200];
    SEXP out;
    SEXP status = ScalarInteger(req->status);

    suv_insert_t * connect = (suv_insert_t *) req->data;
    work_t * w = (work_t *) connect->data;

    if (req->status != 0)
    {
        sprintf(msg, "error handling request: %s", suv_strerror(req->status));
        out = mkString(msg);
    }
    else
    {
        siridb_resp_t * resp = siridb_resp_create(req->pkg, NULL);
        out = print_resp(resp);
        siridb_resp_destroy(resp);
    }

    SEXP call = PROTECT(LCONS(w->cb, LCONS(out, LCONS(status, R_NilValue))));
    R_forceAndCall(call, 2, w->env);
    UNPROTECT(2);


    suv_insert_destroy((suv_insert_t *) req->data);
    siridb_req_destroy(req);
}

static void on_close(void * data, const char * msg)
{
    // printf("%s\n", msg);
}

static void on_error(void * data, const char * msg)
{
    // printf("got an error: %s\n", msg);
}

SEXP print_resp(siridb_resp_t * resp)
{
    SEXP out;
    print_timeit(resp->timeit);

    switch (resp->tp)
    {
    case SIRIDB_RESP_TP_SELECT:
        out = print_select(resp->via.select);
        break;
    case SIRIDB_RESP_TP_LIST:
        out = print_list(resp->via.list); break;
    case SIRIDB_RESP_TP_COUNT:
        out = print_count(resp->via.count); break;
    case SIRIDB_RESP_TP_CALC:
        out = print_calc(resp->via.calc); break;
    case SIRIDB_RESP_TP_SHOW:
        out = print_show(resp->via.show); break;
    case SIRIDB_RESP_TP_SUCCESS:
        out = mkString(resp->via.success); break;
    case SIRIDB_RESP_TP_SUCCESS_MSG:
        out = mkString(resp->via.success_msg); break;
    case SIRIDB_RESP_TP_ERROR:
        out = mkString(resp->via.error); break;
    case SIRIDB_RESP_TP_ERROR_MSG:
        out = mkString(resp->via.error_msg); break;
    default:
        out = mkString("unpack error: unknown response type"); break;
    }
    return out;
}

SEXP print_timeit(siridb_timeit_t * timeit)
{
    if (timeit != NULL)
    {
        // printf("Query time: %f seconds\n", timeit->perfs[timeit->n - 1].time);
        // for (size_t i = 0; i < timeit->n; i++)
        // {
        //     printf(
        //         "    server: %s time: %f\n",
        //         timeit->perfs[i].server,
        //         timeit->perfs[i].time);
        // }
        // printf("\n");
    }
    return R_NilValue;
}

SEXP print_select(siridb_select_t * select)
{
    SEXP nms = allocVector(STRSXP, select->n);
    SEXP columns = allocVector(VECSXP, select->n);
    setAttrib(columns, R_NamesSymbol, nms);

    for (size_t m = 0; m < select->n; m++)
    {
        siridb_series_t * series = select->series[m];

        SEXP points = allocVector(VECSXP, series->n);

        for (size_t i = 0; i < series->n; i++)
        {
            SEXP tv = allocVector(VECSXP, 2);
            SET_VECTOR_ELT(tv, 0, ScalarInteger(series->points[i].ts));

            switch (series->tp)
            {
            case SIRIDB_SERIES_TP_INT64:
                SET_VECTOR_ELT(tv, 1, ScalarInteger(series->points[i].via.int64));
                break;
            case SIRIDB_SERIES_TP_REAL:
                SET_VECTOR_ELT(tv, 1, ScalarReal(series->points[i].via.real));
                break;
            case SIRIDB_SERIES_TP_STR:
                SET_VECTOR_ELT(tv, 1, mkString(series->points[i].via.str));
                break;
            }
            SET_VECTOR_ELT(points, i, tv);
        }

        SET_STRING_ELT(nms, m, mkChar(series->name));
        SET_VECTOR_ELT(columns, m, points);
    }

    return columns;
}

SEXP print_list(siridb_list_t * list)
{
    qp_array_t * headers = list->headers->via.array;

    int n = list->data->via.array->n;
    SEXP rows = allocVector(VECSXP, n);
    for (size_t r = 0; r < list->data->via.array->n; r++)
    {
        qp_array_t * row = list->data->via.array->values[r].via.array;

        SEXP nms = allocVector(STRSXP, row->n);
        SEXP columns = allocVector(VECSXP, row->n);
        setAttrib(columns, R_NamesSymbol, nms);

        for (size_t c = 0; c < row->n; c++)
        {
            SET_STRING_ELT(nms, c, mkChar(headers->values[c].via.str));

            switch (row->values[c].tp)
            {
            case QP_RES_INT64:
                SET_VECTOR_ELT(columns, c, ScalarInteger(row->values[c].via.int64));
                break;
            case QP_RES_REAL:
                SET_VECTOR_ELT(columns, c, ScalarReal(row->values[c].via.real));
                break;
            case QP_RES_STR:
                SET_VECTOR_ELT(columns, c, mkString(row->values[c].via.str));
                break;
            default:
                SET_VECTOR_ELT(columns, c, R_NilValue);
            }
        }

        SET_VECTOR_ELT(rows, r, columns);
    }
    return rows;
}

SEXP print_count(uint64_t count)
{
    SEXP nms = allocVector(STRSXP, 1);
    SEXP columns = allocVector(VECSXP, 1);
    setAttrib(columns, R_NamesSymbol, nms);

    SET_STRING_ELT(nms, 0, mkChar("count"));
    SET_VECTOR_ELT(columns, 0, ScalarInteger(count));
    return columns;
}

SEXP print_calc(uint64_t calc)
{
    SEXP nms = allocVector(STRSXP, 1);
    SEXP columns = allocVector(VECSXP, 1);
    setAttrib(columns, R_NamesSymbol, nms);

    SET_STRING_ELT(nms, 0, mkChar("calc"));
    SET_VECTOR_ELT(columns, 0, ScalarReal(calc));
    return columns;
}

SEXP print_show(siridb_show_t * show)
{
    SEXP nms = allocVector(STRSXP, show->n);
    SEXP columns = allocVector(VECSXP, show->n);
    setAttrib(columns, R_NamesSymbol, nms);

    for (size_t c = 0; c < show->n; c++)
    {
        SET_STRING_ELT(nms, c, mkChar(show->items[c].key));
        switch (show->items[c].value->tp)
        {
        case QP_RES_INT64:
            SET_VECTOR_ELT(columns, c, ScalarInteger(show->items[c].value->via.int64));
            break;
        case QP_RES_REAL:
            SET_VECTOR_ELT(columns, c, ScalarReal(show->items[c].value->via.real));
            break;
        case QP_RES_STR:
            SET_VECTOR_ELT(columns, c, mkString(show->items[c].value->via.str));
            break;
        default:
            SET_VECTOR_ELT(columns, c, R_NilValue);
        }
    }
    return columns;
}


// .C interface
// void fun(double *a, int *b, char **v, int *d);
// static R_NativePrimitiveArgType argfun[] = {  REALSXP, INTSXP, STRSXP, LGLSXP };
static const R_CMethodDef cMethods[] =
{
//    {"SiriDB$fun",   (DL_FUNC) &fun, 4, argfun},
   {NULL, NULL, 0, NULL}
};

// .Call interface
R_CallMethodDef callMethods[]  = {
   {"SiriDB$connect", (DL_FUNC) &sconnect, 3},
   {"SiriDB$close", (DL_FUNC) &sclose, 0},
   {"SiriDB$query", (DL_FUNC) &squery, 3},
   {"SiriDB$insert", (DL_FUNC) &sinsert, 3},
  {NULL, NULL, 0}
};

void R_init_siridbr(DllInfo* info)
{
  R_registerRoutines(info, cMethods, callMethods, NULL, NULL);
  R_useDynamicSymbols(info, TRUE);
}
void R_unload_siridbr(DllInfo *info)
{
  // TODO Release resources.
}