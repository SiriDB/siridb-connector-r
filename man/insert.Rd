\name{SiriDB$insert}
\alias{SiriDB$insert}
\title{
  Insert
}
\description{
  Insert data
  \cr
  Insert time series data into SiriDB. Requires a List with at least one series and a callback function to catch the result.
}
\arguments{
  \item{series}{A List containing the series to insert}
  \item{callback}{A callback function to catch the result.
  \cr
  The callback function will be called with two arguments:
  \enumerate{
  \item A response Object
  \item Number indicating the status. The status is 0 when successful or a negative value in case of an error. (see Status codes for the possible status codes)}
  }
}
\examples{
  series = list(
    list(name='series_i', type='integer', points=list(
      list(timestamp=1500000000L, value=0L),
      list(timestamp=1500000900L, value=1L))),
    list(name='series_f', type='float', points=list(
      list(timestamp=1500000000L, value=0.1))),
    list(name='series_s', type='string', points=list(
      list(timestamp=1500000000L, value='blabla')))
  )

  s <- SiriDB(user="iris", password="siri", dbname="dbtest", server="localhost", port=9000L)
  s$connect(function (err) {
    if (is.null(err)) {
      s$insert(series, function (resp, status) {
          # close the connection
          s$close()
      })
    }
  })
}
