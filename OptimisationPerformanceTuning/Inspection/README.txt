
>  objdump -dtC --visualize-jumps CodeInspection.cpp.o

        CodeInspection.cpp.o:     file format elf64-x86-64

            SYMBOL TABLE:
            0000000000000000 l    df *ABS*	0000000000000000 CodeInspection.cpp
            0000000000000000 l    d  .text	0000000000000000 .text
            0000000000000000 l    d  .data	0000000000000000 .data
            0000000000000000 l    d  .bss	0000000000000000 .bss
            0000000000000000 l    d  .note.GNU-stack	0000000000000000 .note.GNU-stack
            0000000000000000 l    d  .eh_frame	0000000000000000 .eh_frame
            0000000000000000 l    d  .comment	0000000000000000 .comment
            0000000000000000 g     F .text	0000000000000007 add(int)
            0000000000000000 g     O .bss	0000000000000004 value
            0000000000000010 g     F .text	0000000000000010 foo()


        Disassembly of section .text:

            0000000000000000 <add(int)>:
               0:	01 3d 00 00 00 00    	add    %edi,0x0(%rip)        # 6 <add(int)+0x6>
               6:	c3                   	retq
               7:	66 0f 1f 84 00 00 00 	nopw   0x0(%rax,%rax,1)
               e:	00 00

            0000000000000010 <foo()>:
              10:	8b 05 00 00 00 00    	mov    0x0(%rip),%eax        # 16 <foo()+0x6>
              16:	83 c0 03             	add    $0x3,%eax
              19:	89 05 00 00 00 00    	mov    %eax,0x0(%rip)        # 1f <foo()+0xf>
              1f:	c3                   	retq

==============================================================================================================