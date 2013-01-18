mspy
====

MetaSpy is a file metadata search engine

Installation
============

1. cd mspy
2. make

Manual
=======

1. Edit configure file partition.conf for your system or remain it unchanged.
2. Run "mspy-crawl" to crawl metadata from each partition in config file(wait patiently).
3. Run "mspy-search" to start your searching.
   * Every search command start with "query" followed by options.
   
   **options:**
   * --size(-s) min_file_size max_file_size (Bb, Kk, Mm, Gg can be used, -1 means ignoring).
   * --uid(-u) uid    (eg.  query -u 0, search the files for root)
   * --gid(-g) gid    
   * --atime(-a) min_atime max_atime (eg. query -a 2010-06-17 2010-06-19, -1 means ignoring).
   * --ctime(-c) min_ctime max_ctime 
   * --mtime(-m) min_mtime max_mtime
   
 
