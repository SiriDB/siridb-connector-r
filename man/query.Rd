\name{SiriDB$query}
\alias{SiriDB$query}
\title{
  Query
}
\description{
  Query SiriDB
  \cr
  Requires a string containing the query and a callback function to catch the result.
}
\arguments{
  \item{query}{A string containing the query}
  \item{callback}{A callback function to catch the result.
  \cr
  The callback function will be called with two arguments:
  \enumerate{
  \item A response Object
  \item Number indicating the status. The status is 0 when successful or a negative value in case of an error. (see Status codes for the possible status codes)}
  }
}
\examples{
  s = new(SiriDB, "iris", "siri", "dbtest", "localhost", 9000)
  s$connect(function (err) {
    if (is.null(err)) {
      s$query("show", function (resp, status) {
        # do something with response

        # close the connection
        s$close()
      })
    }
  })
}
