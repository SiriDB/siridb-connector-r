#ifndef RQPACK_h
#define RQPACK_h

#include <suv.h>
#include <Rcpp.h>

Rcpp::List on_select(siridb_select_t * select);
Rcpp::List on_list(siridb_list_t * list);
Rcpp::List on_result(siridb_resp_t * resp);
Rcpp::List on_count(uint64_t count);
Rcpp::List on_calc(uint64_t calc);
Rcpp::List on_show(siridb_show_t * show);


#endif