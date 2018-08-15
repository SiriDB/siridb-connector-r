\name{SiriDB$connect}
\alias{SiriDB$connect}
\title{
  Connect
}
\description{
  Connect to SiriDB
  \cr
  A callback function can be used to check if the connect is successful.
}
\arguments{
  \item{callback}{A callback function can be used to check if the connect is successful}
}
\examples{
  s = new(SiriDB, "iris", "siri", "dbtest", "localhost", 9000)
  s$connect(function (err) {
    # success: err is NULL
    # error:   err is a string with an error message
    if (is.null(err)) {
      # close the connection
      s$close()
    }
  })
}
