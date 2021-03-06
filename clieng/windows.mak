#
#  @file windows.mak
#
#  @brief The main makefile for cli engine
#
#  @author Min Zhang
#
#  @note
#
#  

#---------------------------------------------------------------------------------------------------

DLLNAME = jf_clieng
RESOURCE = clieng

SOURCES = clieng.c cmdhistory.c cmdparser.c engio.c

JIUTAI_SRCS = $(JIUTAI_DIR)\jf_mutex.c $(JIUTAI_DIR)\jf_hashtable.c $(JIUTAI_DIR)\jf_hex.c \
    $(JIUTAI_DIR)\jf_process.c

EXTRA_LIBS = $(LIB_DIR)\jf_logger.lib $(LIB_DIR)\jf_string.lib $(LIB_DIR)\jf_files.lib \
    $(LIB_DIR)\jf_jiukun.lib

EXTRA_DEFS = -DJIUFENG_CLIENG_DLL

!include "$(TOPDIR)\mak\winlib.mak"

#---------------------------------------------------------------------------------------------------



