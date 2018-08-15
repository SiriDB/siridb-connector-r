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
      list(timestamp=1500000000, value=0),
      list(timestamp=1500000900, value=1))),
    list(name='series_f', type='float', points=list(
      list(timestamp=1500000000, value=0.1))),
    list(name='series_s', type='string', points=list(
      list(timestamp=1500000000, value='blabla')))
  )

  s = new(SiriDB, "iris", "siri", "dbtest", "localhost", 9000)
  s$connect(function (err) {
    if (is.null(err)) {
      s$insert(series, function (resp, status) {
          # close the connection
          s$close()
      })
    }
  })
}
