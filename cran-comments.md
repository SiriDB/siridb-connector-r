## Test environments
* ubuntu 16.04, R 3.4.4
* windows 7, R-devel (2018-08-13 r75132)

## R CMD check results
0 errors | 1 warnings | 2 notes
* checking CRAN incoming feasibility ... NOTE
Maintainer: ‘Koos Joosten <koos@transceptor.technology>’

New submission
* checking whether package 'siridbr' can be installed ... WARNING
Found the following significant warnings:
  libsuv/../libsiridb/pkg.h:54:24: warning: ISO C++ forbids zero-size array 'data' [-Wpedantic]
* checking top-level files ... NOTE
Files ‘README.md’ or ‘NEWS.md’ cannot be checked without ‘pandoc’ being installed.
Non-standard file/directory found at top level:
  ‘cran-comments.md’