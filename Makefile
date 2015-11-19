# contrib/ntext/Makefile

MODULE_big = ntext
OBJS = ntext_io.o normalize.o similar.o 

EXTENSION = ntext
DATA = ntext--1.0.sql 

REGRESS = ntext

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/ntext
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
