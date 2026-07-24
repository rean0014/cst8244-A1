@    #50.builtin
@    setconf TIMEZONE __OPT_TZ__
#if defined(OPT_ARCH_aarch64le) || defined(OPT_ARCH_armle_v7)
@    # ARM Motherboard Express uATX features a PrimeCell PL031 RTC as per:
@    # - http://infocenter.arm.com/help/topic/com.arm.doc.dui0447j/Caccaigc.html
@    # - http://infocenter.arm.com/help/topic/com.arm.doc.dui0447j/ch04s02s02.html
@    rtc -b 0x1C170000 primecell
#else
@    rtc hw
#endif
@    mount -uo noexec /dev/shmem
@# local/snippets/ifs_start.custom
@# Commands executed from ifs start-up script prior to invoking main start-up script or slm
@    #~30.builtin
#if defined(OPT_SLM_yes)
@    START_P(slm_t) slm -Vvv __IFS_PATH__/slm.cfg &
#else
@    __IFS_PATH__/startup.sh
#endif
@    #~80.builtin
#if defined(OPT_SLM_yes)
@    waitfor /system
#endif
@
@    display_msg "Startup complete"
@    uname -a
@
@    reopen __DEVC_PATH__
