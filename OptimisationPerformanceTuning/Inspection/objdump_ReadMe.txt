
>  objdump -dtC --visualize-jumps main.o

==============================================================================================================

main.o:     file format elf64-x86-64

SYMBOL TABLE:
0000000000000000 l    df *ABS*	0000000000000000 main.cpp
0000000000000000 l    d  .text	0000000000000000 .text
0000000000000000 l    d  .data	0000000000000000 .data
0000000000000000 l    d  .bss	0000000000000000 .bss
0000000000000000 l    d  .debug_info	0000000000000000 .debug_info
0000000000000000 l    d  .debug_abbrev	0000000000000000 .debug_abbrev
0000000000000000 l    d  .debug_aranges	0000000000000000 .debug_aranges
0000000000000000 l    d  .debug_line	0000000000000000 .debug_line
0000000000000000 l    d  .debug_str	0000000000000000 .debug_str
0000000000000000 l    d  .note.GNU-stack	0000000000000000 .note.GNU-stack
0000000000000000 l    d  .eh_frame	0000000000000000 .eh_frame
0000000000000000 l    d  .comment	0000000000000000 .comment
0000000000000000         *UND*	0000000000000000 std::ios_base_library_init()
0000000000000000 g     F .text	0000000000000014 add(int, int)
0000000000000014 g     F .text	0000000000000012 sub(int, int)
0000000000000026 g     F .text	0000000000000013 mul(int, int)
0000000000000039 g     F .text	0000000000000013 divide(int, int)
000000000000004c g     F .text	0000000000000012 main



Disassembly of section .text:

0000000000000000 <add(int, int)>:
   0:	55                   	push   %rbp
   1:	48 89 e5             	mov    %rsp,%rbp
   4:	89 7d fc             	mov    %edi,-0x4(%rbp)
   7:	89 75 f8             	mov    %esi,-0x8(%rbp)
   a:	8b 55 fc             	mov    -0x4(%rbp),%edx
   d:	8b 45 f8             	mov    -0x8(%rbp),%eax
  10:	01 d0                	add    %edx,%eax
  12:	5d                   	pop    %rbp
  13:	c3                   	retq

0000000000000014 <sub(int, int)>:
  14:	55                   	push   %rbp
  15:	48 89 e5             	mov    %rsp,%rbp
  18:	89 7d fc             	mov    %edi,-0x4(%rbp)
  1b:	89 75 f8             	mov    %esi,-0x8(%rbp)
  1e:	8b 45 fc             	mov    -0x4(%rbp),%eax
  21:	2b 45 f8             	sub    -0x8(%rbp),%eax
  24:	5d                   	pop    %rbp
  25:	c3                   	retq

0000000000000026 <mul(int, int)>:
  26:	55                   	push   %rbp
  27:	48 89 e5             	mov    %rsp,%rbp
  2a:	89 7d fc             	mov    %edi,-0x4(%rbp)
  2d:	89 75 f8             	mov    %esi,-0x8(%rbp)
  30:	8b 45 fc             	mov    -0x4(%rbp),%eax
  33:	0f af 45 f8          	imul   -0x8(%rbp),%eax
  37:	5d                   	pop    %rbp
  38:	c3                   	retq

0000000000000039 <divide(int, int)>:
  39:	55                   	push   %rbp
  3a:	48 89 e5             	mov    %rsp,%rbp
  3d:	89 7d fc             	mov    %edi,-0x4(%rbp)
  40:	89 75 f8             	mov    %esi,-0x8(%rbp)
  43:	8b 45 fc             	mov    -0x4(%rbp),%eax
  46:	99                   	cltd
  47:	f7 7d f8             	idivl  -0x8(%rbp)
  4a:	5d                   	pop    %rbp
  4b:	c3                   	retq

000000000000004c <main>:
  4c:	55                   	push   %rbp
  4d:	48 89 e5             	mov    %rsp,%rbp
  50:	89 7d fc             	mov    %edi,-0x4(%rbp)
  53:	48 89 75 f0          	mov    %rsi,-0x10(%rbp)
  57:	b8 00 00 00 00       	mov    $0x0,%eax
  5c:	5d                   	pop    %rbp
  5d:	c3                   	retq

==============================================================================================================