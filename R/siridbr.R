.onLoad <- function(lib, pkg) 
{
    library.dynam("siridbr", pkg, lib)
}

SiriDB <- function(user="iris", password="siri", dbname="dbtest", server="localhost", port=9000)
{

    env_ <- environment()
    connected_ <- FALSE
    user_ <- user
    password_ <- password
    dbname_ = dbname
    server_ <- server
    port_ <- port
    
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
            .Call('sconnect', env_, list(u=user_, p=password_, d=dbname_, s=server_, p=port_), function (x) {
                assign("connected_", TRUE, env_)
                f(x)
            })
            return(NULL)
        },
        query = function (query, f) {
            .Call('squery', env_, list(q=query), f)
            return(NULL)
        },
        insert = function (series, f) {
            .Call('sinsert', env_, series, f)
            return(NULL)
        },
        close = function () {
            .Call('sclose')
            return(NULL)
        }
    )

    assign('this', me, envir=env_)
    class(me) <- append(class(me), "SiriDB_")
    return(me)
}