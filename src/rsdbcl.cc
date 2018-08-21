#include "rsdbcl.h"
#include "rresp.h"
#include "libuv/include/uv.h"


void SiriDB::connect(Rcpp::Function f) {
      struct sockaddr_in addr;

      uv_loop_init(&loop);

      uv_ip4_addr(server_.c_str(), port_, &addr);

      siridb = siridb_create();
      /* handle siridb == NULL */
      /* Warning: do not use siridb->data since it will be used by libsuv */

      buf = suv_buf_create(siridb);
      /* handle buf == NULL */

      /* set optional callback functions */
      buf->onclose = on_close;
      buf->onerror = on_error;

      siridb_req_t * req = siridb_req_create(siridb, connect_cb, NULL);
      /* handle req == NULL */

      suv_connect_t * connect = suv_connect_create(req, user_.c_str(), password_.c_str(), dbname_.c_str());
      /* handle connect == NULL */

      connect->data = &f;
      req->data = (void *) connect;

      suv_connect(&loop, connect, buf, (struct sockaddr *) &addr);

      uv_run(&loop, UV_RUN_DEFAULT);

      /* cleanup buffer */
      suv_buf_destroy(buf);

      /* cleanup siridb */
      siridb_destroy(siridb);

      /* close uv loop */
      uv_loop_close(&loop);

      return;
  }

void SiriDB::close() {
    suv_close(buf, NULL);

    return;
}

void SiriDB::query(std::string q, Rcpp::Function f)
{
    siridb_req_t * req = siridb_req_create(siridb, query_cb, NULL);
    /* handle req == NULL */

    suv_query_t * suvquery = suv_query_create(req, (char *) q.c_str());
    /* handle suvquery == NULL */

    /* bind suvquery to req->data */
    suvquery->data = &f;
    req->data = (void *) suvquery;

    suv_query(suvquery);
    /* check query_cb for errors */

    return;
}

void SiriDB::insert(Rcpp::List series, Rcpp::Function cb)
{
    size_t n = (size_t) series.length();
    size_t m;
    std::vector<siridb_series_t *> iseries;
    siridb_series_t * iserie;
    siridb_point_t * ipoint;

    for (size_t i = 0; i < n; i++)
    {
        Rcpp::List s = Rcpp::List((SEXP) series[i]);
        std::string name = Rcpp::as<std::string>(s["name"]);
        std::string typ = Rcpp::as<std::string>(s["type"]);
        Rcpp::List pts = Rcpp::List((SEXP) s["points"]);

        siridb_series_e tp;
        if (!strcmp(typ.c_str(), "integer"))
        {
            tp = SIRIDB_SERIES_TP_INT64;
        }
        else if (!strcmp(typ.c_str(), "float"))
        {
            tp = SIRIDB_SERIES_TP_REAL;
        }
        else if (!strcmp(typ.c_str(), "string"))
        {
            tp = SIRIDB_SERIES_TP_STR;
        }
        else
        {
            Rcpp::stop("incorrect datatype");
        }

        m = (size_t) pts.length();

        iserie = siridb_series_create(
            tp,
            name.c_str(),
            m);

        for (size_t j = 0; j < m; j++)
        {
            Rcpp::List pt = Rcpp::List((SEXP) pts[j]);

            ipoint = iserie->points + j;
            ipoint->ts = (uint64_t) pt["timestamp"];

            switch (tp)
            {
            case SIRIDB_SERIES_TP_INT64:
                ipoint->via.int64 = (int64_t) pt["value"];
                break;
            case SIRIDB_SERIES_TP_REAL:
                ipoint->via.real = (double_t) pt["value"];
                break;
            case SIRIDB_SERIES_TP_STR:
                std::string val = Rcpp::as<std::string>(pt["value"]);
                ipoint->via.str = (char *) strdup(val.c_str());
                break;
            }
        }
        iseries.push_back(iserie);
    }

    siridb_req_t * req = siridb_req_create(siridb, insert_cb, NULL);
    /* handle req == NULL */

    suv_insert_t * suvinsert = suv_insert_create(req, &iseries[0], n);
    /* handle suvinsert == NULL */

    /* cleanup the series, we don't need them anymore */
    for (   std::vector<siridb_series_t *>::iterator it = iseries.begin();
            it != iseries.end();
            ++it) {
        siridb_series_destroy(*it);
    }

    /* bind suvinsert to qreq->data */
    suvinsert->data = &cb;
    req->data = (void *) suvinsert;

    suv_insert(suvinsert);
    /* check insert_cb for errors */
}

void SiriDB::connect_cb(siridb_req_t * req)
{
    suv_connect_t * connect = (suv_connect_t *) req->data;
    /* handle connect == NULL (for example in case the request is cancelled
     * before a connection handle was attached) */

    Rcpp::Function * cb = (Rcpp::Function *) connect->data;

    if (req->status)
    {
        char buff[200];
        sprintf(buff, "connect failed: %s", suv_strerror(req->status));
        (*cb)(buff);
    }
    else
    {
        switch (req->pkg->tp)
        {
        case CprotoResAuthSuccess:
            (*cb)(R_NilValue);
            break;
        case CprotoErrAuthCredentials:
            (*cb)("auth failed: invalid credentials");
            suv_close(suv_buf_from_req(req), NULL);
            break;
        case CprotoErrAuthUnknownDb:
            (*cb)("auth failed: unknown database");
            suv_close(suv_buf_from_req(req), NULL);
            break;
        default:
            char buff[40];
            sprintf(buff, "auth failed: unknown error (%u)", req->pkg->tp);
            (*cb)(buff);
            suv_close(suv_buf_from_req(req), NULL);
        }
    }

    /* destroy suv_connect_t */
    suv_connect_destroy(connect);

    /* destroy siridb request */
    siridb_req_destroy(req);
}

void SiriDB::query_cb(siridb_req_t * req)
{
    suv_query_t * suvquery = (suv_query_t *) req->data;

    Rcpp::Function * cb = (Rcpp::Function *) suvquery->data;


    if (req->status != 0)
    {
        (*cb)(R_NilValue, req->status);
    }
    else
    {
        siridb_resp_t * resp = siridb_resp_create(req->pkg, NULL);

        Rcpp::List result = on_result(resp);
        (*cb)(result, req->status);
        siridb_resp_destroy(resp);
    }

    /* destroy suv_query_t */
    suv_query_destroy(suvquery);

    /* destroy siridb request */
    siridb_req_destroy(req);
}

void SiriDB::insert_cb(siridb_req_t * req)
{
    suv_insert_t * connect = (suv_insert_t *) req->data;
    Rcpp::Function * cb = (Rcpp::Function *) connect->data;

    if (req->status != 0)
    {
        char buff[200];
        sprintf(buff, "error handling request: %s", suv_strerror(req->status));
        (*cb)(R_NilValue, req->status);
    }
    else
    {
        int rc;
        siridb_resp_t * resp = siridb_resp_create(req->pkg, &rc);

        if (rc != 0) {
            char buff[49];
            sprintf(buff, "error handling request: error code(%d)", rc);
            (*cb)(buff, req->status);
        } else {
            Rcpp::List result = on_result(resp);
            (*cb)(result, req->status);

        }

        siridb_resp_destroy(resp);
    }

    /* destroy suv_insert_t */
    suv_insert_destroy((suv_insert_t *) req->data);

    /* destroy siridb request */
    siridb_req_destroy(req);
}

void SiriDB::on_close(void * data, const char * msg)
{
    // printf("%s\n", msg);
}

void SiriDB::on_error(void * data, const char * msg)
{
    // printf("got an error: %s\n", msg);
}







// Expose the classes
RCPP_MODULE(RSiriDBClient) {
  using namespace Rcpp;

  class_<SiriDB>( "SiriDB")
    .constructor<std::string,std::string,std::string,std::string,int>("Constructor")
    .method("connect", &SiriDB::connect)
    .method("close", &SiriDB::close)
    .method("query", &SiriDB::query)
    .method("insert", &SiriDB::insert)
  ;
}