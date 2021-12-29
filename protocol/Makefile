#ENCRYPT_DISABLE=1
INCLUDES=. ${TOPDIR}/src/rtu/include ${TOPDIR}/src/rtu/ ${TOPDIR}/src/devices/ ${TOPDIR}/src/cli/  ${TOPDIR}/src/rtu/log ${TOPDIR}/src/rtu/storage/

lib=rtuprotocol
EXT_CFLAGS=-D_GNU_SOURCE -D_rtu_CLI_ -fpic
ifndef ENCRYPT_DISABLE
INCLUDES+=${TOPDIR}/src/encrypt
#EXT_CFLAGS+=-D_ENCRYPT_ENABLE_
endif

include ${TOPDIR}/src/make/Make.build
include ${TOPDIR}/src/make/Make.lib.so
