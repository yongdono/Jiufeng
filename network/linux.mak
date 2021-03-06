#
#  @file Makefile
#
#  @brief The makefile for network library
#
#  @author Min Zhang
#
#  @note
#
#  

#---------------------------------------------------------------------------------------------------

SONAME = jf_network

SOURCES = internalsocket.c socket.c socketpair.c \
    chain.c utimer.c asocket.c assocket.c acsocket.c \
    adgram.c resolve.c network.c

JIUTAI_SRCS = jf_mutex.c jf_time.c

EXTRA_LIBS = -ljf_logger -ljf_ifmgmt -ljf_jiukun

ifeq ("$(DEBUG_JIUFENG)", "yes")
#    EXTRA_CFLAGS += -DDEBUG_CHAIN
#    EXTRA_CFLAGS += -DDEBUG_UTIMER
#    EXTRA_CFLAGS += -DDEBUG_ASOCKET
endif

include $(TOPDIR)/mak/lnxlib.mak

#---------------------------------------------------------------------------------------------------

