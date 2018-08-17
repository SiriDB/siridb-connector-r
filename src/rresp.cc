#include "rresp.h"


Rcpp::List on_select(siridb_select_t * select) {
    Rcpp::List result;

    for (size_t m = 0; m < select->n; m++)
    {
        siridb_series_t * series = select->series[m];

        Rcpp::List points(series->n);

        for (size_t i = 0; i < series->n; i++)
        {
            switch (series->tp)
            {
            case SIRIDB_SERIES_TP_INT64:
                points[i] = Rcpp::List::create(Rcpp::Named("ts") = series->points[i].ts, Rcpp::Named("value") = series->points[i].via.int64);
                break;
            case SIRIDB_SERIES_TP_REAL:
                points[i] = Rcpp::List::create(Rcpp::Named("ts") = series->points[i].ts, Rcpp::Named("value") = series->points[i].via.real);
                break;
            case SIRIDB_SERIES_TP_STR:
                points[i] = Rcpp::List::create(Rcpp::Named("ts") = series->points[i].ts, Rcpp::Named("value") = (Rcpp::StringVector) series->points[i].via.str);
                break;
            }

        }

        result[series->name] = points;
    }
    return result;
}

Rcpp::List on_list(siridb_list_t * list)
{
    qp_array_t * headers = list->headers->via.array;

    Rcpp::List rows(list->data->via.array->n);
    for (size_t r = 0; r < list->data->via.array->n; r++)
    {
        qp_array_t * row = list->data->via.array->values[r].via.array;

        Rcpp::List columns;
        for (size_t c = 0; c < row->n; c++)
        {
            switch (row->values[c].tp)
            {
            case QP_RES_INT64:
                columns[headers->values[c].via.str] = row->values[c].via.int64;
                break;
            case QP_RES_REAL:
                columns[headers->values[c].via.str] = row->values[c].via.real;
                break;
            case QP_RES_STR:
                columns[headers->values[c].via.str] = (Rcpp::StringVector) row->values[c].via.str;
                break;
            default: assert(0);
            }
        }

        rows[r] = columns;
    }
    return rows;
}

Rcpp::List on_count(uint64_t count)
{
    return Rcpp::List::create(Rcpp::Named("count") = count);
}

Rcpp::List on_calc(uint64_t calc)
{
    return Rcpp::List::create(Rcpp::Named("calculated") = calc);
}

Rcpp::List on_show(siridb_show_t * show)
{

    Rcpp::List columns;
    for (size_t i = 0; i < show->n; i++)
    {

        switch (show->items[i].value->tp)
        {
        case QP_RES_INT64:
            columns[show->items[i].key] = show->items[i].value->via.int64;
            break;
        case QP_RES_REAL:
            columns[show->items[i].key] = show->items[i].value->via.real;
            break;
        case QP_RES_STR:
            columns[show->items[i].key] = (Rcpp::StringVector) show->items[i].value->via.str;
            break;
        default: assert(0);
        }
    }

    return columns;
}

Rcpp::List on_result(siridb_resp_t * resp)
{
    // print_timeit(resp->timeit);
    Rcpp::List result;

    switch (resp->tp)
    {
    case SIRIDB_RESP_TP_SELECT:
        result = on_select(resp->via.select);
        break;
    case SIRIDB_RESP_TP_LIST:
        result = on_list(resp->via.list);
        break;
    case SIRIDB_RESP_TP_COUNT:
        result = on_count(resp->via.count);
        break;
    case SIRIDB_RESP_TP_CALC:
        result = on_calc(resp->via.calc);
        break;
    case SIRIDB_RESP_TP_SHOW:
        result = on_show(resp->via.show);
        break;
    case SIRIDB_RESP_TP_SUCCESS:
        result["message"] = (Rcpp::StringVector) resp->via.success;
        // print_msg(resp->via.success);
        break;
    case SIRIDB_RESP_TP_SUCCESS_MSG:
        result["message"] = (Rcpp::StringVector) resp->via.success_msg;
        // print_msg(resp->via.success_msg);
        break;
    case SIRIDB_RESP_TP_ERROR:
        result["message"] = (Rcpp::StringVector) resp->via.error;
        // print_msg(resp->via.error);
        break;
    case SIRIDB_RESP_TP_ERROR_MSG:
        result["message"] = (Rcpp::StringVector) resp->via.error_msg;
        // print_msg(resp->via.error_msg);
        break;
    default: assert(0);
    }

    if (resp->timeit != NULL) {
        Rcpp::List timeits;
        for (size_t i = 0; i < resp->timeit->n; i++)
        {
            timeits[resp->timeit->perfs[i].server] = resp->timeit->perfs[i].time;
        }
        result["timeit"] = timeits;
    }

    return result;
}