\name{SiriDB$close}
\alias{SiriDB$close}
\title{
  Close
}
\description{
  Close the connection
}
\examples{
  s = new(SiriDB, "iris", "siri", "dbtest", "localhost", 9000)
  s$connect(function (err) {
    if (is.null(err)) {
      s$close()
    }
  })
}
