.onLoad <- function(lib, pkg)
{
    library.dynam("siridbr", pkg, lib)
}

SiriDB <- function(user, password, dbname, server, port=9000L)
{

    env_ <- environment()
    connected_ <- FALSE
    user_ <- user
    password_ <- password
    dbname_ = dbname
    server_ <- server
    port_ <- port
    errcodes <- list(
        ERR_UNKNOWN_DB = -73,
        ERR_MSG = -64,
        ERR_QUERY = -65,
        ERR_INSERT = -66,
        ERR_SERVER = -67,
        ERR_POOL = -68,
        ERR_ACCESS = -69,
        ERR_RUNTIME = -70,
        ERR_NOT_AUTHENTICATED = -71,
        ERR_CREDENTIALS = -72,
        ERR_UNKNOWN_DB = -73,
        ERR_LOADING_DB = -74
    )
    version <- "0.1.0"

    me <- list(
        env_ = env_,
        env = function()
        {
            return(get("env_", env_))
        },
        connected = function()
        {
            return(get("connected_", env_))
        },
        connect = function (f) {
            .Call('sconnect', env_, list(u=user_, p=password_, d=dbname_, s=server_, p=port_), function (resp, status) {
                if (status==0) {
                    if (is.null(resp)) {
                        assign("connected_", TRUE, env_)
                    } else {
                        .Call('sclose')
                    }
                }
                f(resp)
            })
            return(NULL)
        },
        query = function (query, f) {
            .Call('squery', env_, list(q=query), function (resp, status) {
                f(resp, status)
            })
            return(NULL)
        },
        insert = function (series, f) {
            .Call('sinsert', env_, series, function (resp, status) {
                f(resp, status)
            })
            return(NULL)
        },
        close = function () {
            .Call('sclose')
            assign("connected_", FALSE, env_)
            return(NULL)
        }
    )

    assign('this', me, envir=env_)
    class(me) <- append(class(me), "SiriDB_")
    return(me)
}

