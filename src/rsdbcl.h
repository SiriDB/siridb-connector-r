#ifndef RSDBCL_h
#define RSDBCL_h

#include <suv.h>
#include <Rcpp.h>

static uv_loop_t loop;

class SiriDB {
private:
    siridb_t * siridb;
    suv_buf_t * buf;

    std::string user_;
    std::string password_;
    std::string dbname_;
    std::string server_;
    int port_;

    static void connect_cb(siridb_req_t * req);
    static void query_cb(siridb_req_t * req);
    static void insert_cb(siridb_req_t * req);

    static void on_close(void * data, const char * msg);
    static void on_error(void * data, const char * msg);
public:
    SiriDB(std::string user, std::string password, std::string dbname, std::string server, int port) {
        user_ = user;
        password_ = password;
        dbname_ = dbname;
        server_ = server;
        port_ = port;
    }

    void connect(Rcpp::Function cb);
    void query(std::string q, Rcpp::Function cb);
    void insert(Rcpp::List series, Rcpp::Function cb);
    void close();
};


#endif