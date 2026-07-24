#define __SYS_TRUST_OPT__
#define __PAM_SECPOL_CONFIG__
#define __SYS_SIZE__ 40960
#define __SYS_INODES__ 1000
#define __DATA_SIZE__ 122880
#define __DATA_INODES__ 3000
#define __TOOL_DIR__ /cygdrive/C//Users/david/qnx710/host/win64/x86_64/../../common/mkqnximage
#define __EXTRA_PROC_OPTIONS__ 
#define __MKFS_PATH__ ${MKFS_PATH}
#define __LD_QNX_IFS_LINK__ procmgr_symlink __IFS_PATH__/__LD_QNX__ /usr/lib/__LD_QNX__
#define __DEVB_NAME__ devb-eide
#define __DEVC_NAME__ devc-con
#define __DEVC_PATH__ /dev/con1
#define __DEVC_PATH2__ /dev/con2
#define __DEVC_PATH3__ /dev/con3
#define __DEVC_PATH4__ /dev/con4
@
#if defined(OPT_ARCH_x86_64)
#if defined(OPT_BOOT_MODE_uefi)
#define __VIRTUAL__ x86_64,uefi
#else
#define __VIRTUAL__ x86_64,bios
#endif
#define __LD_QNX__ ldqnx-64.so.2
#define __STARTUP__ startup-x86
#else
#define __VIRTUAL__ x86,bios
#define __LD_QNX__ ldqnx.so.2
#define __STARTUP__ startup-x86
#endif
@
#define __IOPKT_OPTS__
#define __BOOT_ADDR__ 20m
#define __DEVB_OPTS__
#define __DEVC_OPTS__
#define __HAS_PCI__
#define __IOPKT_VIRTIO_OPTS__
@
#define __NETDRIVER__ e1000
#define __NETDEVICE__ wm0
@
#define __PCI_HW_CONFIG_FILE__
@
