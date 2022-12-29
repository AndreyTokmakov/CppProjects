/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define to enable AVX-512F buffers. */
/* #undef AVX512F_FOUND */

/* Define to set the specific CPU L1 cache-line size, in bytes. */
#define CACHELINE_SIZE 64

/* Define this if you have any supported netlink library required */
#define CONFIG_LIBNL 1

/* Define this if libnl-tiny is present on your system */
/* #undef CONFIG_LIBNL20 */

/* Define this if libnl-3.0 is present on your system */
#define CONFIG_LIBNL30 1

/* Algorithm AES in gcrypt library */
/* #undef GCRYPT_WITH_AES */

/* Algorithm ARCFOUR in gcrypt library */
/* #undef GCRYPT_WITH_ARCFOUR */

/* Algorithm BLOWFISH in gcrypt library */
/* #undef GCRYPT_WITH_BLOWFISH */

/* Algorithm CAST5 in gcrypt library */
/* #undef GCRYPT_WITH_CAST5 */

/* Algorithm CMAC_AES in gcrypt library */
/* #undef GCRYPT_WITH_CMAC_AES */

/* Algorithm CRC in gcrypt library */
/* #undef GCRYPT_WITH_CRC */

/* Algorithm DES in gcrypt library */
/* #undef GCRYPT_WITH_DES */

/* Algorithm DSA in gcrypt library */
/* #undef GCRYPT_WITH_DSA */

/* Algorithm ELGAMAL in gcrypt library */
/* #undef GCRYPT_WITH_ELGAMAL */

/* Algorithm HAVAL in gcrypt library */
/* #undef GCRYPT_WITH_HAVAL */

/* Algorithm IDEA in gcrypt library */
/* #undef GCRYPT_WITH_IDEA */

/* Algorithm MD2 in gcrypt library */
/* #undef GCRYPT_WITH_MD2 */

/* Algorithm MD4 in gcrypt library */
/* #undef GCRYPT_WITH_MD4 */

/* Algorithm MD5 in gcrypt library */
/* #undef GCRYPT_WITH_MD5 */

/* Algorithm RFC2268 in gcrypt library */
/* #undef GCRYPT_WITH_RFC2268 */

/* Algorithm RMD160 in gcrypt library */
/* #undef GCRYPT_WITH_RMD160 */

/* Algorithm RSA in gcrypt library */
/* #undef GCRYPT_WITH_RSA */

/* Algorithm SERPENT in gcrypt library */
/* #undef GCRYPT_WITH_SERPENT */

/* Algorithm SHA0 in gcrypt library */
/* #undef GCRYPT_WITH_SHA0 */

/* Algorithm SHA1 in gcrypt library */
/* #undef GCRYPT_WITH_SHA1 */

/* Algorithm SHA224 in gcrypt library */
/* #undef GCRYPT_WITH_SHA224 */

/* Algorithm SHA256 in gcrypt library */
/* #undef GCRYPT_WITH_SHA256 */

/* Algorithm SHA384 in gcrypt library */
/* #undef GCRYPT_WITH_SHA384 */

/* Algorithm SHA512 in gcrypt library */
/* #undef GCRYPT_WITH_SHA512 */

/* Algorithm TIGER in gcrypt library */
/* #undef GCRYPT_WITH_TIGER */

/* Algorithm TWOFISH in gcrypt library */
/* #undef GCRYPT_WITH_TWOFISH */

/* Algorithm WHIRLPOOL in gcrypt library */
/* #undef GCRYPT_WITH_WHIRLPOOL */

/* Define if your system has sys/auxv.h header */
/* #undef HAS_AUXV */

/* Define if you have AirPcap. */
/* #undef HAVE_AIRPCAP */

/* Define to 1 if you have the `aligned_alloc' function. */
/* #undef HAVE_ALIGNED_ALLOC */

/* Define to 1 if you have the <bsd/string.h> header file. */
/* #undef HAVE_BSD_STRING_H */

/* define if the compiler supports basic C++11 syntax */
/* #undef HAVE_CXX11 */

/* define if the compiler supports basic C++14 syntax */
/* #undef HAVE_CXX14 */

/* define if the compiler supports basic C++17 syntax */
#define HAVE_CXX17 1

/* Define to 1 if you have the <dirent.h> header file. */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if fseeko (and presumably ftello) exists and is declared. */
#define HAVE_FSEEKO 1

/* Gcrypt library is available */
/* #undef HAVE_GCRYPT */

/* Define to 1 if you have the <getopt.h> header file. */
#define HAVE_GETOPT_H 1

/* Define if you have hwloc library. */
#define HAVE_HWLOC 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define to 1 if you support file names longer than 14 characters. */
#define HAVE_LONG_FILE_NAMES 1

/* Define to 1 if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define to 1 if you have the `memalign' function. */
/* #undef HAVE_MEMALIGN */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if you have openssl/cmac.h header present. */
#define HAVE_OPENSSL_CMAC_H 1

/* Define to 1 if you have the <openssl/crypto.h> header file. */
#define HAVE_OPENSSL_CRYPTO_H 1

/* Define this if you have libpcap on your system */
#define HAVE_PCAP 1

/* Define to 1 if you have the <pcap.h> header file. */
#define HAVE_PCAP_H 1

/* Define this if you have libpcre on your system */
#define HAVE_PCRE 1

/* Define to 1 if you have the `posix_memalign' function. */
#define HAVE_POSIX_MEMALIGN 1

/* Define if pthread_{,attr_}{g,s}etaffinity_np is supported. */
#define HAVE_PTHREAD_AFFINITY_NP 1

/* Have PTHREAD_PRIO_INHERIT. */
#define HAVE_PTHREAD_PRIO_INHERIT 1

/* Define if you have sqlite3 */
#define HAVE_SQLITE 1

/* Have the SQLITE3 library */
#define HAVE_SQLITE3 1

/* Define to 1 if you have the <sqlite3.h> header file. */
#define HAVE_SQLITE3_H 1

/* Define to 1 if you have the <stdarg.h> header file. */
#define HAVE_STDARG_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strlcat' function. */
/* #undef HAVE_STRLCAT */

/* Define to 1 if you have the `strlcpy' function. */
/* #undef HAVE_STRLCPY */

/* Define to 1 if you have the <sys/auxv.h> header file. */
/* #undef HAVE_SYS_AUXV_H */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define if you have zlib */
#define HAVE_ZLIB 1

/* Define to 1 if you have the `_aligned_malloc' function. */
/* #undef HAVE__ALIGNED_MALLOC */

/* Define to 1 if you have the `__mingw_aligned_malloc' function. */
/* #undef HAVE___MINGW_ALIGNED_MALLOC */

/* The Cygwin DLL version string suffix */
#define LT_CYGWIN_VER "-1-7-0.dll"

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Define if your OpenSSL has an AES_128_cbc implementation. */
#define OPENSSL_WITH_AES 1

/* Define if your OpenSSL has an ARC4 implementation. */
#define OPENSSL_WITH_ARCFOUR 1

/* Define if your OpenSSL has a CMAC implementation. */
#define OPENSSL_WITH_CMAC 1

/* Define if your OpenSSL has an MD5 implementation. */
#define OPENSSL_WITH_MD5 1

/* Define if your OpenSSL has an SHA-1 implementation. */
#define OPENSSL_WITH_SHA1 1

/* Define if your OpenSSL has an SHA-256 implementation. */
#define OPENSSL_WITH_SHA256 1

/* Name of package */
#define PACKAGE "aircrack-ng"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "https://forum.aircrack-ng.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "aircrack-ng"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "aircrack-ng 1.7.0_rev-0bd4677c"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "aircrack-ng"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.7.0_rev-0bd4677c"

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* The size of `off_t', as computed by sizeof. */
#define SIZEOF_OFF_T 8

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "1.7.0_rev-0bd4677c"

/* Define if portable WIN32 is supported */
/* #undef WIN32_PORTABLE */

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define this if 64-bit file access requires this define to be present */
#define _LARGEFILE64_SOURCE 1

/* Define to 1 to make fseeko visible on some hosts (e.g. glibc 2.2). */
/* #undef _LARGEFILE_SOURCE */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* The version information of the project */
#define _REVISION "1.7.0_rev-0bd4677c"

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */
