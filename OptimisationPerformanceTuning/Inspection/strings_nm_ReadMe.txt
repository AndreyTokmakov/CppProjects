

# 1. Compile --> 'app' binary filename

>  g++-15.1 main.cpp -o app

        or

>  make clean all


# 2. The strings utility, when applied to a binary file,
#    prints all human-readable character sequences present in that file.

>  strings app | grep add

    _Z3addii

>  strings app | grep func

    # No out put

-------------------------------------------------------------------------------------

>  nm -C app | grep -i add

0000000000401106 T add(int, int)

>  nm -C app | grep -i sub

000000000040111a T sub(int, int)

