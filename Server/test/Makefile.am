#AM_FLAGS = -I$(top_srcdir)/include/common
#lib_LIBRARIES = libtest.a
#libtest_a_SOURCES = \
	test.cpp
bin_PROGRAMS = ../bin/test
___bin_test_SOURCES = test.cpp
___bin_test_LDADD = $(top_srcdir)/lib/common/libcomm.a $(top_srcdir)/lib/echo/libecho.a -lpthread
AM_CPPFLAGS = -I$(top_srcdir)/include/common -I$(top_srcdir)/include/echo -lpthread