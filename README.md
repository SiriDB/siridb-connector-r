# SiriDB Connector R
Connector for communicating with a [SiriDB](https://github.com/SiriDB/siridb-server#readme) node

---------------------------------------
  * [Installation](#installation)
  * [Quick usage](#quick-usage)
  * [SiriDBClient](#siridbclient)
    * [connect](#siridbclientconnect)
    * [query](#siridbclientquery)
    * [insert](#siridbclientinsert)
    * [close](#siridbclientclose)
  * [Events](#events)
  * [Status codes](#status-codes)
  * [Version info](#version-info)

---------------------------------------

## Installation
```{r}
install.packages('siridbr')
```

## Quick usage
```{r}
library(siridbr)

siridb <- SiriDB(user="iris", password="siri", dbname="dbtest", server="localhost", port=9000L)

siridb$connect(function(err) {
    if (!is.null(err)) {
        cat('Connection error: ', err)
    } else {
        siridb$close()
    }
})
```

## SiriDBClient
Create a new SiriDB Client. This creates a new client but `connect()` must be used to connect.
```{r}
siridb <- SiriDB(
    user="iris",         # database user
    password="siri",     # password
    dbname="dbtest",     # database name
    server="localhost",  # server address
    port=9000L           # server port
)
```

### SiriDBClient.connect
Connect to SiriDB. A callback function can be used to check if the connect is successful.
```{r}
siridb$connect(function(err) {
    # success: err is NULL
    # error:   err is a string with an error message
    if (!is.null(err)) {
        cat('Connection error: ', err)
    }
})
```

### SiriDBClient.query
Query SiriDB. Requires a string containing the query and a callback function to catch the result.

The callback function will be called with two arguments:
 - first argument: A response Object
 - second argument: Number indicating the status. The status is 0 when successful or a negative value in case of an error.
   (see [Status codes](#status-codes) for the possible status codes)

```{r}
siridb$query("select * from /.*series/", function(resp, status) {
    // success: status is 0 and resp is an Object containing the data
    // error:   status < 0 and resp.error_msg contains a description about the error
    if (status) {
        cat('Query error: ', resp$error_msg, status)
    } else {
        cat(resp)
    }
})
```

### SiriDBClient.insert
Insert time series data into SiriDB. Requires an Array with at least one series Object.string containing the query and a callback function to catch the result.

The callback function will be called with two arguments:
 - first argument: A response Object
 - second argument: Number indicating the status. The status is 0 when successful or a negative value in case of an error.
   (see [Status codes](#status-codes) for the possible status codes)

```{r}
series = list(
    list(
        name='example',     # name
        points=list(
            list(timestamp=1500000000, value=0L),   # time-stamp, value
            list(timestamp=1500000900, value=1L)    # etc.
        )
    )
)

siridb$insert(series, function(resp, status) {
    // success: status is 0 and resp.success_msg contains a description about the successful insert
    // error:   status < 0 and resp.error_msg contains a description about the error
    if (status) {
        cat('Insert error: ', resp$error_msg, status)
    } else {
        cat(resp.success_msg)   # insert message
    }
})
```

### SiriDBClient.close
Close the connection.
```{r}
siridb$close()
```

## Events
TODO

## Status codes
Sometimes its useful to act on a specific error, for example you might want to retry the request in case of `ERR_SERVER` while a `ERR_INSERT` error indicates something is wrong with the data.

The following status codes can be returned:

- `SiriDB.errcodes.ERR_MSG` (-64) *General error*
- `SiriDB.errcodes.ERR_QUERY` (-65) *Most likely a syntax error in the query*
- `SiriDB.errcodes.ERR_INSERT` (-66) *Most likely the data is invalid or corrupt*
- `SiriDB.errcodes.ERR_SERVER` (-67) *The server could not perform the request, you could try another SiriDB server*
- `SiriDB.errcodes.ERR_POOL` (-68) *At least one pool has no online SiriDB server*
- `SiriDB.errcodes.ERR_ACCESS` (-69) *The database user has not enough privileges to process the request*,
- `SiriDB.errcodes.ERR_RUNTIME` (-70) *Unexpected error has occurred, please check the SiriDB log*
- `SiriDB.errcodes.ERR_NOT_AUTHENTICATED` (-71) *The connection is not authenticated*
- `SiriDB.errcodes.ERR_CREDENTIALS` (-72) *Credentials are invalid*
- `SiriDB.errcodes.ERR_UNKNOWN_DB` (-73) *Trying to authenticate to an unknown database*
- `SiriDB.errcodes.ERR_LOADING_DB` (-74) *The database is loading*

## Version info
Show version info.
```{r}
siridb$version()
```