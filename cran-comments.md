## Test environments
* ubuntu 16.04, R 3.4.4
* windows 7, R-devel (2018-08-13 r75132)

## R CMD check results
0 errors | 0 warnings | 0 notes

## R CMD check results Linux

* checking CRAN incoming feasibility ... NOTE
Maintainer: ‘Koos Joosten <koos@transceptor.technology>’

New submission
* checking top-level files ... NOTE
Files ‘README.md’ or ‘NEWS.md’ cannot be checked without ‘pandoc’ being installed.
Non-standard file/directory found at top level:
  ‘cran-comments.md’


## R CMD check results Windows

* checking whether package 'siridbr' can be installed ... WARNING
Found the following significant warnings:
  libsuv/../libsiridb/queue.h:38:24: warning: ISO C++ forbids zero-size array 'nodes' [-Wpedantic]
  libsuv/../libsiridb/../libqpack/qpack.h:200:31: warning: ISO C++ forbids zero-size array 'nesting' [-Wpedantic]
  libsuv/../libsiridb/series.h:51:27: warning: ISO C++ forbids zero-size array 'points' [-Wpedantic]
  libsuv/../libsiridb/pkg.h:54:24: warning: ISO C++ forbids zero-size array 'data' [-Wpedantic]
  libsuv/../libsiridb/resp.h:102:30: warning: ISO C++ forbids zero-size array 'series' [-Wpedantic]
  libsuv/../libsiridb/resp.h:108:25: warning: ISO C++ forbids zero-size array 'items' [-Wpedantic]
  libsuv/../libsiridb/resp.h:114:25: warning: ISO C++ forbids zero-size array 'perfs' [-Wpedantic]
  rsdbcl.cc:77:32: warning: ISO C++ forbids variable length array 'iseries' [-Wvla]
* checking top-level files ... NOTE
Non-standard file/directory found at top level:
  'cran-comments.md'