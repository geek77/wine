/*
 * Copyright 2012 Piotr Caban for CodeWeavers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifdef __i386__  /* thiscall functions are i386-specific */

#define THISCALL(func) __thiscall_ ## func
#define THISCALL_NAME(func) __ASM_NAME("__thiscall_" #func)
#define __thiscall __stdcall
#define DEFINE_THISCALL_WRAPPER(func,args) \
    extern void THISCALL(func)(void); \
    __ASM_GLOBAL_FUNC(__thiscall_ ## func, \
                      "popl %eax\n\t" \
                      "pushl %ecx\n\t" \
                      "pushl %eax\n\t" \
                      "jmp " __ASM_NAME(#func) __ASM_STDCALL(args) )
#else /* __i386__ */

#define THISCALL(func) func
#define THISCALL_NAME(func) __ASM_NAME(#func)
#define __thiscall __cdecl
#define DEFINE_THISCALL_WRAPPER(func,args) /* nothing */

#endif /* __i386__ */

#ifdef _WIN64

#define VTABLE_ADD_FUNC(name) "\t.quad " THISCALL_NAME(name) "\n"

#define __ASM_VTABLE(name,funcs) \
    __asm__(".data\n" \
            "\t.align 8\n" \
            "\t.quad " __ASM_NAME(#name "_rtti") "\n" \
            "\t.globl " __ASM_NAME("MSVCRT_" #name "_vtable") "\n" \
            __ASM_NAME("MSVCRT_" #name "_vtable") ":\n" \
            funcs "\n\t.text")

#else

#define VTABLE_ADD_FUNC(name) "\t.long " THISCALL_NAME(name) "\n"

#define __ASM_VTABLE(name,funcs) \
    __asm__(".data\n" \
            "\t.align 4\n" \
            "\t.long " __ASM_NAME(#name "_rtti") "\n" \
            "\t.globl " __ASM_NAME("MSVCRT_" #name "_vtable") "\n" \
            __ASM_NAME("MSVCRT_" #name "_vtable") ":\n" \
            funcs "\n\t.text")

#endif /* _WIN64 */

#define DEFINE_RTTI_DATA(name, off, base_classes, cl1, cl2, cl3, cl4, cl5, cl6, cl7, cl8, cl9, mangled_name) \
    static const type_info name ## _type_info = { \
        &MSVCRT_type_info_vtable, \
        NULL, \
        mangled_name \
    }; \
\
static const rtti_base_descriptor name ## _rtti_base_descriptor = { \
    &name ##_type_info, \
    base_classes, \
    { 0, -1, 0}, \
    64 \
}; \
\
static const rtti_base_array name ## _rtti_base_array = { \
    { \
        &name ## _rtti_base_descriptor, \
        cl1, \
        cl2, \
        cl3, \
        cl4, \
        cl5, \
        cl6, \
        cl7, \
        cl8, \
        cl9, \
    } \
}; \
\
static const rtti_object_hierarchy name ## _hierarchy = { \
    0, \
    0, \
    base_classes+1, \
    &name ## _rtti_base_array \
}; \
\
const rtti_object_locator name ## _rtti = { \
    0, \
    off, \
    0, \
    &name ## _type_info, \
    &name ## _hierarchy \
}

#define DEFINE_RTTI_DATA0(name, off, mangled_name) \
    DEFINE_RTTI_DATA(name, off, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, mangled_name)
#define DEFINE_RTTI_DATA1(name, off, cl1, mangled_name) \
    DEFINE_RTTI_DATA(name, off, 1, cl1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, mangled_name)
#define DEFINE_RTTI_DATA2(name, off, cl1, cl2, mangled_name) \
    DEFINE_RTTI_DATA(name, off, 2, cl1, cl2, NULL, NULL, NULL, NULL, NULL, NULL, NULL, mangled_name)
#define DEFINE_RTTI_DATA3(name, off, cl1, cl2, cl3, mangled_name) \
    DEFINE_RTTI_DATA(name, off, 3, cl1, cl2, cl3, NULL, NULL, NULL, NULL, NULL, NULL, mangled_name)
#define DEFINE_RTTI_DATA4(name, off, cl1, cl2, cl3, cl4, mangled_name) \
    DEFINE_RTTI_DATA(name, off, 4, cl1, cl2, cl3, cl4, NULL, NULL, NULL, NULL, NULL, mangled_name)
#define DEFINE_RTTI_DATA8(name, off, cl1, cl2, cl3, cl4, cl5, cl6, cl7, cl8, mangled_name) \
    DEFINE_RTTI_DATA(name, off, 8, cl1, cl2, cl3, cl4, cl5, cl6, cl7, cl8, NULL, mangled_name)
#define DEFINE_RTTI_DATA9(name, off, cl1, cl2, cl3, cl4, cl5, cl6, cl7, cl8, cl9, mangled_name) \
    DEFINE_RTTI_DATA(name, off, 9, cl1, cl2, cl3, cl4, cl5, cl6, cl7, cl8, cl9, mangled_name)

/* rtti */
typedef struct _rtti_base_descriptor
{
    const type_info *type_descriptor;
    int num_base_classes;
    this_ptr_offsets offsets;    /* offsets for computing the this pointer */
    unsigned int attributes;
} rtti_base_descriptor;

typedef struct _rtti_base_array
{
    const rtti_base_descriptor *bases[10]; /* First element is the class itself */
} rtti_base_array;

typedef struct _rtti_object_hierarchy
{
    unsigned int signature;
    unsigned int attributes;
    int array_len; /* Size of the array pointed to by 'base_classes' */
    const rtti_base_array *base_classes;
} rtti_object_hierarchy;

typedef struct _rtti_object_locator
{
    unsigned int signature;
    int base_class_offset;
    unsigned int flags;
    const type_info *type_descriptor;
    const rtti_object_hierarchy *type_hierarchy;
} rtti_object_locator;