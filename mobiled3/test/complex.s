	.file	"complex.cc"
	.local	_ZSt8__ioinit
	.comm	_ZSt8__ioinit,1,1
	.text
	.align 2
	.p2align 4,,15
.globl _ZN3foo5polarEdd
	.type	_ZN3foo5polarEdd,@function
_ZN3foo5polarEdd:
.LFB1:
	pushl	%ebp
.LCFI0:
	movl	%esp, %ebp
.LCFI1:
	pushl	%edi
.LCFI2:
	pushl	%esi
.LCFI3:
	pushl	%ebx
.LCFI4:
	subl	$20, %esp
.LCFI5:
	movl	24(%ebp), %esi
	movl	20(%ebp), %ebx
	pushl	%esi
	pushl	%ebx
	movl	8(%ebp), %edi
.LCFI6:
	call	cos
	popl	%eax
	popl	%edx
	pushl	%esi
	fldl	12(%ebp)
	fmulp	%st, %st(1)
	pushl	%ebx
	fstpl	-24(%ebp)
	call	sin
	fmull	12(%ebp)
	fldl	-24(%ebp)
	fstpl	(%edi)
	fstpl	8(%edi)
	addl	$16, %esp
	leal	-12(%ebp), %esp
	popl	%ebx
	popl	%esi
	movl	%edi, %eax
	popl	%edi
	popl	%ebp
	ret	$4
.LFE1:
.Lfe1:
	.size	_ZN3foo5polarEdd,.Lfe1-_ZN3foo5polarEdd
	.align 2
	.p2align 4,,15
.globl main
	.type	main,@function
main:
.LFB2:
	pushl	%ebp
.LCFI7:
	movl	%esp, %ebp
.LCFI8:
	pushl	%edi
.LCFI9:
	pushl	%esi
.LCFI10:
	pushl	%ebx
.LCFI11:
	subl	$300, %esp
.LCFI12:
	andl	$-16, %esp
	pushl	%esi
	leal	-256(%ebp), %eax
	pushl	%eax
	leal	-264(%ebp), %eax
	leal	-56(%ebp), %edi
	pushl	%eax
	pushl	%edi
	movl	$0, -256(%ebp)
	movl	$0, -252(%ebp)
	movl	$0, -264(%ebp)
	movl	$1072693248, -260(%ebp)
.LCFI13:
	call	_ZSt5polarIdESt7complexIT_ERKS1_S4_
	movl	-56(%ebp), %eax
	popl	%ecx
	movl	%eax, -40(%ebp)
	movl	-48(%ebp), %eax
	popl	%ebx
	movl	%eax, -32(%ebp)
	leal	-272(%ebp), %eax
	pushl	%eax
	leal	-280(%ebp), %eax
	movl	-52(%ebp), %edx
	pushl	%eax
	movl	%edx, -36(%ebp)
	pushl	%edi
	movl	-44(%ebp), %edx
	movl	%edx, -28(%ebp)
	movl	$1413754136, -272(%ebp)
	movl	$1074340347, -268(%ebp)
	movl	$0, -280(%ebp)
	movl	$1072693248, -276(%ebp)
	call	_ZSt5polarIdESt7complexIT_ERKS1_S4_
	xorl	%ebx, %ebx
	xorl	%esi, %esi
	popl	%eax
	fldl	-56(%ebp)
	fldl	-48(%ebp)
	fxch	%st(1)
	popl	%edx
	fstl	-72(%ebp)
	fxch	%st(1)
	fstl	-64(%ebp)
	movl	%ebx, -88(%ebp)
	movl	%esi, -84(%ebp)
	movl	%ebx, -80(%ebp)
	movl	%esi, -76(%ebp)
#APP
	##########################################
#NO_APP
	fldl	-40(%ebp)
	fldl	-32(%ebp)
	fld	%st(1)
	fld	%st(1)
	fxch	%st(1)
	fmul	%st(5), %st
	fxch	%st(1)
	fmul	%st(4), %st
	fxch	%st(3)
	fmulp	%st, %st(4)
	fxch	%st(4)
	fmulp	%st, %st(1)
	fxch	%st(3)
	fsubp	%st, %st(1)
	fxch	%st(1)
	faddp	%st, %st(2)
	fstl	-120(%ebp)
	fxch	%st(1)
	fstl	-112(%ebp)
	fxch	%st(1)
	fstl	-104(%ebp)
	fxch	%st(1)
	fstl	-96(%ebp)
	fxch	%st(1)
	fstpl	-88(%ebp)
	fstpl	-80(%ebp)
#APP
	##########################################
#NO_APP
	leal	-288(%ebp), %eax
	pushl	%eax
	leal	-296(%ebp), %eax
	pushl	%eax
	pushl	%edi
	movl	%ebx, -288(%ebp)
	movl	%esi, -284(%ebp)
	movl	$0, -296(%ebp)
	movl	$1072693248, -292(%ebp)
	call	_ZSt5polarIdESt7complexIT_ERKS1_S4_
	popl	%ecx
	popl	%eax
	movl	-56(%ebp), %eax
	movl	%eax, -136(%ebp)
	movl	-48(%ebp), %eax
	movl	%eax, -128(%ebp)
	leal	-304(%ebp), %eax
	pushl	%eax
	leal	-312(%ebp), %eax
	movl	-52(%ebp), %edx
	pushl	%eax
	movl	%edx, -132(%ebp)
	pushl	%edi
	movl	-44(%ebp), %edx
	movl	%edx, -124(%ebp)
	movl	$1413754136, -304(%ebp)
	movl	$1074340347, -300(%ebp)
	movl	$0, -312(%ebp)
	movl	$1072693248, -308(%ebp)
	call	_ZSt5polarIdESt7complexIT_ERKS1_S4_
	fldl	-56(%ebp)
	fldl	-48(%ebp)
	fxch	%st(1)
	fstl	-152(%ebp)
	fxch	%st(1)
	fstl	-144(%ebp)
#APP
	##########################################
#NO_APP
	fldl	-136(%ebp)
	fldl	-128(%ebp)
	fld	%st(1)
	fld	%st(1)
	fxch	%st(1)
	fmul	%st(5), %st
	fxch	%st(1)
	fmul	%st(4), %st
	fxch	%st(3)
	fmulp	%st, %st(4)
	fxch	%st(4)
	fmulp	%st, %st(1)
	fxch	%st(3)
	fsubp	%st, %st(1)
	fxch	%st(1)
	faddp	%st, %st(2)
	fstl	-184(%ebp)
	fxch	%st(1)
	fstl	-176(%ebp)
	fxch	%st(1)
	fstpl	-168(%ebp)
	fstpl	-160(%ebp)
#APP
	##########################################
#NO_APP
	pushl	%esi
	pushl	%ebx
	pushl	$1072693248
	leal	-200(%ebp), %eax
	pushl	$0
	pushl	%eax
.LCFI14:
	call	_ZN3foo5polarEdd
	addl	$16, %esp
	pushl	$1074340347
	pushl	$1413754136
	pushl	$1072693248
	leal	-216(%ebp), %eax
	pushl	$0
	pushl	%eax
	call	_ZN3foo5polarEdd
	addl	$28, %esp
	movl	%ebx, -224(%ebp)
	movl	%esi, -220(%ebp)
	movl	%ebx, -232(%ebp)
	movl	%esi, -228(%ebp)
#APP
	##########################################
#NO_APP
	fldl	-200(%ebp)
	fldl	-192(%ebp)
	fldl	-216(%ebp)
	fldl	-208(%ebp)
	fld	%st(3)
	fld	%st(3)
	fxch	%st(1)
	fmul	%st(3), %st
	fxch	%st(1)
	fmul	%st(2), %st
	fsubrp	%st, %st(1)
	fstpl	-248(%ebp)
	fmulp	%st, %st(3)
	fmulp	%st, %st(1)
	movl	-248(%ebp), %eax
	faddp	%st, %st(1)
	fstpl	-240(%ebp)
	movl	%eax, -232(%ebp)
	movl	-244(%ebp), %eax
	movl	%eax, -228(%ebp)
	movl	-240(%ebp), %eax
	movl	%eax, -224(%ebp)
	movl	-236(%ebp), %eax
	movl	%eax, -220(%ebp)
#APP
	##########################################
#NO_APP
	leal	-12(%ebp), %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	xorl	%eax, %eax
	popl	%ebp
	ret
.LFE2:
.Lfe2:
	.size	main,.Lfe2-main
	.section	.gnu.linkonce.t._ZSt5polarIdESt7complexIT_ERKS1_S4_,"ax",@progbits
	.align 2
	.p2align 4,,15
	.weak	_ZSt5polarIdESt7complexIT_ERKS1_S4_
	.type	_ZSt5polarIdESt7complexIT_ERKS1_S4_,@function
_ZSt5polarIdESt7complexIT_ERKS1_S4_:
.LFB3:
	pushl	%ebp
.LCFI15:
	movl	%esp, %ebp
.LCFI16:
	pushl	%edi
.LCFI17:
	pushl	%esi
.LCFI18:
	pushl	%ebx
.LCFI19:
	subl	$20, %esp
.LCFI20:
	movl	16(%ebp), %esi
	movl	4(%esi), %eax
	pushl	%eax
	movl	(%esi), %ecx
	pushl	%ecx
	movl	8(%ebp), %ebx
	movl	12(%ebp), %edi
.LCFI21:
	call	cos
	popl	%eax
	popl	%edx
	movl	4(%esi), %eax
	fmull	(%edi)
	pushl	%eax
	movl	(%esi), %eax
	pushl	%eax
	fstpl	-24(%ebp)
	call	sin
	fmull	(%edi)
	fldl	-24(%ebp)
	fstpl	(%ebx)
	fstpl	8(%ebx)
	addl	$16, %esp
	leal	-12(%ebp), %esp
	movl	%ebx, %eax
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret	$4
.LFE3:
.Lfe3:
	.size	_ZSt5polarIdESt7complexIT_ERKS1_S4_,.Lfe3-_ZSt5polarIdESt7complexIT_ERKS1_S4_
	.text
	.align 2
	.p2align 4,,15
	.type	_Z41__static_initialization_and_destruction_0ii,@function
_Z41__static_initialization_and_destruction_0ii:
.LFB4:
	pushl	%ebp
.LCFI22:
	movl	%esp, %ebp
.LCFI23:
	subl	$8, %esp
.LCFI24:
	cmpl	$65535, 12(%ebp)
	je	.L20
.L18:
	movl	%ebp, %esp
	popl	%ebp
	ret
	.p2align 4,,15
.L20:
	cmpl	$1, 8(%ebp)
	jne	.L18
	subl	$12, %esp
	pushl	$_ZSt8__ioinit
.LCFI25:
	call	_ZNSt8ios_base4InitC1Ev
	addl	$12, %esp
	pushl	$__dso_handle
	pushl	$0
	pushl	$__tcf_0
	call	__cxa_atexit
	addl	$16, %esp
	jmp	.L18
.LFE4:
.Lfe4:
	.size	_Z41__static_initialization_and_destruction_0ii,.Lfe4-_Z41__static_initialization_and_destruction_0ii
	.align 2
	.p2align 4,,15
	.type	__tcf_0,@function
__tcf_0:
.LFB5:
	pushl	%ebp
.LCFI26:
	movl	%esp, %ebp
.LCFI27:
	movl	$_ZSt8__ioinit, 8(%ebp)
	popl	%ebp
.LCFI28:
	jmp	_ZNSt8ios_base4InitD1Ev
.LFE5:
.Lfe5:
	.size	__tcf_0,.Lfe5-__tcf_0
	.align 2
	.p2align 4,,15
	.type	_GLOBAL__I__ZN3foo5polarEdd,@function
_GLOBAL__I__ZN3foo5polarEdd:
.LFB6:
	pushl	%ebp
.LCFI29:
	movl	%esp, %ebp
.LCFI30:
	subl	$16, %esp
.LCFI31:
	pushl	$65535
	pushl	$1
.LCFI32:
	call	_Z41__static_initialization_and_destruction_0ii
	addl	$16, %esp
	movl	%ebp, %esp
	popl	%ebp
	ret
.LFE6:
.Lfe6:
	.size	_GLOBAL__I__ZN3foo5polarEdd,.Lfe6-_GLOBAL__I__ZN3foo5polarEdd
	.section	.ctors,"aw",@progbits
	.align 4
	.long	_GLOBAL__I__ZN3foo5polarEdd
	.weak	pthread_mutex_unlock
	.weak	pthread_mutex_trylock
	.weak	pthread_mutex_lock
	.weak	pthread_create
	.weak	pthread_setspecific
	.weak	pthread_getspecific
	.weak	pthread_key_delete
	.weak	pthread_key_create
	.weak	pthread_once
	.section	.eh_frame,"aw",@progbits
.Lframe1:
	.long	.LECIE1-.LSCIE1
.LSCIE1:
	.long	0x0
	.byte	0x1
	.string	"zP"
	.uleb128 0x1
	.sleb128 -4
	.byte	0x8
	.uleb128 0x5
	.byte	0x0
	.long	__gxx_personality_v0
	.byte	0xc
	.uleb128 0x4
	.uleb128 0x4
	.byte	0x88
	.uleb128 0x1
	.align 4
.LECIE1:
.LSFDE3:
	.long	.LEFDE3-.LASFDE3
.LASFDE3:
	.long	.LASFDE3-.Lframe1
	.long	.LFB2
	.long	.LFE2-.LFB2
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI7-.LFB2
	.byte	0xe
	.uleb128 0x8
	.byte	0x85
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI8-.LCFI7
	.byte	0xd
	.uleb128 0x5
	.byte	0x4
	.long	.LCFI12-.LCFI8
	.byte	0x83
	.uleb128 0x5
	.byte	0x86
	.uleb128 0x4
	.byte	0x87
	.uleb128 0x3
	.byte	0x4
	.long	.LCFI13-.LCFI12
	.byte	0x2e
	.uleb128 0x10
	.byte	0x4
	.long	.LCFI14-.LCFI13
	.byte	0x2e
	.uleb128 0x20
	.align 4
.LEFDE3:
.LSFDE7:
	.long	.LEFDE7-.LASFDE7
.LASFDE7:
	.long	.LASFDE7-.Lframe1
	.long	.LFB4
	.long	.LFE4-.LFB4
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI22-.LFB4
	.byte	0xe
	.uleb128 0x8
	.byte	0x85
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI23-.LCFI22
	.byte	0xd
	.uleb128 0x5
	.byte	0x4
	.long	.LCFI25-.LCFI23
	.byte	0x2e
	.uleb128 0x10
	.align 4
.LEFDE7:
.LSFDE9:
	.long	.LEFDE9-.LASFDE9
.LASFDE9:
	.long	.LASFDE9-.Lframe1
	.long	.LFB5
	.long	.LFE5-.LFB5
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI26-.LFB5
	.byte	0xe
	.uleb128 0x8
	.byte	0x85
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI27-.LCFI26
	.byte	0xd
	.uleb128 0x5
	.byte	0x4
	.long	.LCFI28-.LCFI27
	.byte	0x2e
	.uleb128 0x4
	.align 4
.LEFDE9:
.LSFDE11:
	.long	.LEFDE11-.LASFDE11
.LASFDE11:
	.long	.LASFDE11-.Lframe1
	.long	.LFB6
	.long	.LFE6-.LFB6
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI29-.LFB6
	.byte	0xe
	.uleb128 0x8
	.byte	0x85
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI30-.LCFI29
	.byte	0xd
	.uleb128 0x5
	.byte	0x4
	.long	.LCFI32-.LCFI30
	.byte	0x2e
	.uleb128 0x10
	.align 4
.LEFDE11:
	.ident	"GCC: (GNU) 3.2"
