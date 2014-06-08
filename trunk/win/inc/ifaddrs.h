#ifndef _IFADDRS_H_
#define _IFADDRS_H_

struct ifaddrs
{
  struct ifaddrs *ifa_next;	/** Pointer to the next structure.  */

  char *ifa_name;		/** Name of this network interface.  */
  unsigned int ifa_flags;	/** Flags as from SIOCGIFFLAGS ioctl.  */

  struct sockaddr *ifa_addr;	/** Network address of this interface.  */
  struct sockaddr *ifa_netmask; /** Netmask of this interface.  */
  union
  {
    /** At most one of the following two is valid.  If the IFF_BROADCAST
       bit is set in `ifa_flags', then `ifa_broadaddr' is valid.  If the
       IFF_POINTOPOINT bit is set, then `ifa_dstaddr' is valid.
       It is never the case that both these bits are set at once.  */
    struct sockaddr *ifu_broadaddr; /** Broadcast address of this interface. */
    struct sockaddr *ifu_dstaddr; /** Point-to-point destination address.  */
  } ifa_ifu;
  /** These very same macros are defined by <net/if.h> for `struct ifaddr'.
     So if they are defined already, the existing definitions will be fine.  */
# ifndef ifa_broadaddr
#  define ifa_broadaddr	ifa_ifu.ifu_broadaddr
# endif
# ifndef ifa_dstaddr
#  define ifa_dstaddr	ifa_ifu.ifu_dstaddr
# endif

  void *ifa_data;		/** Address-specific data (may be unused).  */
};

#ifdef __cplusplus
extern "C"
{
#endif

int getifaddrs(struct ifaddrs **__ifap);
void freeifaddrs(struct ifaddrs *__ifa);

#ifdef __cplusplus
}
#endif

#endif

