## Note about the types of documentation in BioCro {-}

There several categories of documentation for BioCro:

* The top-level `README.md` is for general information about BioCro
  and is intended for _potential_ users of the package.

* The files in `man` directory document the R functions and the data
  associated with the BioCro package and are intended for _users_ of
  the package.

* The files in the `vignettes` directory are generally "longform"
  documentation, also intended for _users_ of the package.

* There is the documentation generated by Doxygen, generally from
  comments in the C++ source files.  This is of interest to
  developers, but general users may also be interested, particularly
  in the documentation of the various BioCro modules.

* Finally, there are various `.md` or `.Rmd` files scattered about the
  package that are referenced by the `bookdown/_bookdown.yml`
  configuration file, and that can be compiled into a book (_this_
  book, if you are reading this page in _bookdown_) using the
  `bookdown` package.  This documentation is targeted at BioCro
  developers and maintainers.