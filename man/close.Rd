\name{SiriDB$close}
\alias{SiriDB$close}
\title{
  Close
}
\description{
  Close the connection
}
\examples{
  s <- SiriDB(user="iris", password="siri", dbname="dbtest", server="localhost", port=9000L)
  s$connect(function (err) {
    if (is.null(err)) {
      s$close()
    }
  })
}
