include ${TOPDIR}/src/make/Make.local
TARGET=plserver
	
STATIC_LIBS=${TOPDIR}/src/cli/libcli.a \
			${TOPDIR}/src/usermanage/libusermanage.a 
#			${TOPDIR}/src/usbmng/libusbmng.a \
#			${TOPDIR}/src/k37/libk37mng.a \
#			${TOPDIR}/src/threads_comm/libthreads_comm.a \
#			${TOPDIR}/src/devices/libdevices.a \
#			${TOPDIR}/src/k37/application_upgrade/libapplication_upgrade.a \
#			${TOPDIR}/src/k37/system_upgrade/libsystem_upgrade.a \
#			${TOPDIR}/src/k37/usb_port_monitor/libusb_port_monitor.a

			
DYNAMIC_LIBS=/lib/i386-linux-gnu/libcrypt.so.1
#DYNAMIC_LIBS=${TOPDIR}/src/k37/protocol/libk37protocol.so \
             ${TOPDIR}/src/k37/rep_protocol/libk37repprotocol.so \
             ${TOPDIR}/src/k37/mon_protocol/libk37monprotocol.so
 
#STATIC_LIBS=${TOPDIR}/src/SerialDataMng/libSerialDataMng.a 
###NO ppclient######
#STATIC_LIBS+=${TOPDIR}/src/ppclient/libppclient.a  
#STATIC_LIBS+=${TOPDIR}/src/watchdog/libwtd.a	\
	 ${TOPDIR}/src/misc/libmisc.a		\
	 ${TOPDIR}/src/encrypt/encrypt.a 

#######switch static libaray######
#SWITCH_LIBS=${TOPDIR}/src/switch/sysinit/libsysinit.a \
			${TOPDIR}/src/switch/acl/libacl.a \
			${TOPDIR}/src/switch/igmpsnooping/libigmpsnoop.a \
			${TOPDIR}/src/switch/bridge/libbridge.a \
			${TOPDIR}/src/switch/route/libroute.a \
			${TOPDIR}/src/switch/cli/libcli.a \
			${TOPDIR}/src/switch/usermanage/libusermanage.a \
			${TOPDIR}/src/switch/dhcp82/libdhcp82.a \
			${TOPDIR}/src/switch/loopback/libloopback.a \
			${TOPDIR}/src/switch/mac/libmac.a \
			${TOPDIR}/src/switch/mirror/libmirror.a \
			${TOPDIR}/src/switch/port/libport.a \
			${TOPDIR}/src/switch/pppoe/libpppoe.a \
			${TOPDIR}/src/switch/qos/libqos.a \
			${TOPDIR}/src/switch/rstp/librstp.a \
			${TOPDIR}/src/switch/ssp/libssp.a \
			${TOPDIR}/src/switch/sys/libsys.a \
			${TOPDIR}/src/switch/timer/libtimer.a \
			${TOPDIR}/src/switch/vlan/liblinux_vlan.a \
			${TOPDIR}/src/switch/zip/libzip.a \
			${TOPDIR}/src/switch/oam/liboam.a \
			${TOPDIR}/src/ppclient/libppclient.a


####cli and usermanage are in directory switch, but don't remove them because
####they may ge used in future when it is no need to manage the switch.
####No ppclient now
#subdirs=cli usermanage SerialDataMng ppclient watchdog misc modules udhcp encrypt switch
#subdirs=SerialDataMng ppclient watchdog misc modules udhcp encrypt switch 
subdirs = cli \
	usermanage \
	usbmng 
#	threads_comm \
	devices \
	k37 \
	k37/protocol \
	k37/rep_protocol \
	k37/mon_protocol \
	k37/application_upgrade \
	k37/system_upgrade \
	k37/usb_port_monitor
include ${TOPDIR}/src/make/Make.subdirs
include ${TOPDIR}/src/make/Make.build

all:  ${TARGET}_nostrip ${TARGET} sys_init
sys_init:sys_init.c
	${CC} sys_init.o -o sys_init_nostrip
	${STRIP} sys_init_nostrip -o sys_init

${TARGET}:${TARGET}_nostrip 
	${STRIP} ${TARGET}_nostrip -o $@
#	cp ${TARGET} ../kernel/fs/ramdisk/usr/ 
	
#${TARGET}_nostrip:main.o ${STATIC_LIBS} ${SWITCH_LIBS} ${SWITCH_SDK_LIBS}  sys_init.o
#	${CC} main.o -lcrypt -lpthread -lm ${STATIC_LIBS}  ${SWITCH_LIBS} ${SWITCH_SDK_LIBS} -o $@
${TARGET}_nostrip:main.o ${STATIC_LIBS}  sys_init.o
	${CC} main.o -lpthread  -lm -lrt -lgcrypt -ludev ${STATIC_LIBS} ${DYNAMIC_LIBS} -o $@
	#${CC} main.o -lcrypt -lpthread -lsqlite3 -lm -lrt -lgcrypt -ludev ${STATIC_LIBS} -o $@
	#${CC} main.o -lcrypt -lpthread -lsqlite3 -lm -lrt -lgcrypt -ludev ${STATIC_LIBS} ${DYNAMIC_LIBS} -o $@
	
clean::
	rm  -rf *.o ${TARGET} ${TARGET}_nostrip sys_init sys_init_nostrip
                                                
