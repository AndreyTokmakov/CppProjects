
>  sudo apt install dwarves

========================================================================================================
                                    GCC Setup
========================================================================================================

export GCC_VERSION=13.2
export GCC_PATH=/home/andtokm/DiskS/Utils/bin/gcc-$GCC_VERSION

export PATH=${GCC_PATH}/bin:${PATH}
export LD_LIBRARY_PATH=${GCC_PATH}/lib64
export CC=gcc-$GCC_VERSION CXX=g++-$GCC_VERSION


========================================================================================================
                                    Example 1
========================================================================================================

> g++-13.2 -g -c main.cpp -fpack-struct=2
> pahole -S main.o -C StructOne


struct StructOne {
	int                        a;                    /*     0     4 */
	double                     b;                    /*     4     8 */

	/* size: 12, cachelines: 1, members: 2 */
	/* last cacheline: 12 bytes */
} __attribute__((__packed__));


========================================================================================================
                                    Example 2
========================================================================================================

> make clean all
> pahole -S main.o -C StructOne

struct StructOne {
	int                        a;                    /*     0     4 */

	/* XXX 4 bytes hole, try to pack */

	double                     b;                    /*     8     8 */

	/* size: 16, cachelines: 1, members: 2 */
	/* sum members: 12, holes: 1, sum holes: 4 */
	/* last cacheline: 16 bytes */
};

========================================================================================================
                                    Example 3
========================================================================================================

> make clean all
> pahole -S main.o --prettify

struct StructOne
{
	int                        a;                    /*     0     4 */

	/* XXX 4 bytes hole, try to pack */

	double                     b;                    /*     8     8 */

	/* size: 16, cachelines: 1, members: 2 */
	/* sum members: 12, holes: 1, sum holes: 4 */
	/* last cacheline: 16 bytes */
};


struct StructOnePacked1
{
	int                        a;                    /*     0     4 */
	double                     b;                    /*     4     8 */

	/* size: 12, cachelines: 1, members: 2 */
	/* last cacheline: 12 bytes */
} __attribute__((__packed__));


struct StructOnePacked2
{
	int                        a;                    /*     0     4 */
	double                     b;                    /*     4     8 */

	/* size: 12, cachelines: 1, members: 2 */
	/* last cacheline: 12 bytes */
} __attribute__((__packed__));
