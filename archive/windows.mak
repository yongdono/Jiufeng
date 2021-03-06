#
#  @file windows.mak
#
#  @brief The main Makefile for archive library
#
#  @author Min Zhang
#
#  @note
#
#  

#---------------------------------------------------------------------------------------------------

DLLNAME = olarchive
RESOURCE = archive

SOURCES = archive.c arfile.c create.c extract.c
JIUTAI_SRCS = $(JIUTAI_DIR)\linklist.c

EXTRA_LIBS = $(LIB_DIR)\jf_logger.lib $(LIB_DIR)\jf_files.lib $(LIB_DIR)\jf_jiukun.lib \
             $(LIB_DIR)\jf_string.lib

EXTRA_INC_DIR =

EXTRA_DEFS = -DJIUFENG_ARCHIVE_DLL

!include "$(TOPDIR)\mak\winlib.mak"

#---------------------------------------------------------------------------------------------------


