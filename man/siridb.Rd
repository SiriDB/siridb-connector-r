\name{SiriDB}
\alias{SiriDB}
\title{
  A SiriDB Connector for R
}
\description{
  This R module can be used to connect to a SiriDB cluster.
  This creates a new client but $connect() must be used to connect.
}
\arguments{
  \item{user}{Database user}
  \item{password}{Password}
  \item{dbname}{Database name}
  \item{host}{Server address}
  \item{port}{Server port}
}
\examples{
  s = new(SiriDB, user="iris", password="siri", dbname="dbtest", host="localhost", port=9000)
}
