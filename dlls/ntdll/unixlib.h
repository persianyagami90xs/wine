/*
 * Ntdll Unix interface
 *
 * Copyright (C) 2020 Alexandre Julliard
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

#ifndef __NTDLL_UNIXLIB_H
#define __NTDLL_UNIXLIB_H

#include "wine/server.h"
#include "wine/debug.h"

struct ldt_copy;
struct msghdr;

/* increment this when you change the function table */
#define NTDLL_UNIXLIB_VERSION 19

struct unix_funcs
{
    /* Nt* functions */
    NTSTATUS      (WINAPI *NtAllocateVirtualMemory)( HANDLE process, PVOID *ret, ULONG_PTR zero_bits,
                                                     SIZE_T *size_ptr, ULONG type, ULONG protect );
    NTSTATUS      (WINAPI *NtAreMappedFilesTheSame)(PVOID addr1, PVOID addr2);
    NTSTATUS      (WINAPI *NtClose)( HANDLE handle );
    TEB *         (WINAPI *NtCurrentTeb)(void);
    NTSTATUS      (WINAPI *NtDuplicateObject)( HANDLE source_process, HANDLE source,
                                               HANDLE dest_process, HANDLE *dest,
                                               ACCESS_MASK access, ULONG attributes, ULONG options );
    NTSTATUS      (WINAPI *NtFlushVirtualMemory)( HANDLE process, LPCVOID *addr_ptr,
                                                  SIZE_T *size_ptr, ULONG unknown );
    NTSTATUS      (WINAPI *NtFreeVirtualMemory)( HANDLE process, PVOID *addr_ptr,
                                                 SIZE_T *size_ptr, ULONG type );
    NTSTATUS      (WINAPI *NtGetContextThread)( HANDLE handle, CONTEXT *context );
    NTSTATUS      (WINAPI *NtGetWriteWatch)( HANDLE process, ULONG flags, PVOID base, SIZE_T size,
                                             PVOID *addresses, ULONG_PTR *count, ULONG *granularity );
    NTSTATUS      (WINAPI *NtLockVirtualMemory)( HANDLE process, PVOID *addr, SIZE_T *size, ULONG unknown );
    NTSTATUS      (WINAPI *NtMapViewOfSection)( HANDLE handle, HANDLE process, PVOID *addr_ptr,
                                                ULONG_PTR zero_bits, SIZE_T commit_size,
                                                const LARGE_INTEGER *offset_ptr, SIZE_T *size_ptr,
                                                SECTION_INHERIT inherit, ULONG alloc_type, ULONG protect );
    NTSTATUS      (WINAPI *NtProtectVirtualMemory)( HANDLE process, PVOID *addr_ptr, SIZE_T *size_ptr,
                                                    ULONG new_prot, ULONG *old_prot );
    NTSTATUS      (WINAPI *NtQuerySection)( HANDLE handle, SECTION_INFORMATION_CLASS class,
                                            void *ptr, SIZE_T size, SIZE_T *ret_size );
    NTSTATUS      (WINAPI *NtQueryVirtualMemory)( HANDLE process, LPCVOID addr,
                                                  MEMORY_INFORMATION_CLASS info_class,
                                                  PVOID buffer, SIZE_T len, SIZE_T *res_len );
    NTSTATUS      (WINAPI *NtReadVirtualMemory)( HANDLE process, const void *addr, void *buffer,
                                                 SIZE_T size, SIZE_T *bytes_read );
    NTSTATUS      (WINAPI *NtResetWriteWatch)( HANDLE process, PVOID base, SIZE_T size );
    NTSTATUS      (WINAPI *NtSetContextThread)( HANDLE handle, const CONTEXT *context );
    NTSTATUS      (WINAPI *NtSetLdtEntries)( ULONG sel1, LDT_ENTRY entry1, ULONG sel2, LDT_ENTRY entry2 );
    NTSTATUS      (WINAPI *NtUnlockVirtualMemory)( HANDLE process, PVOID *addr,
                                                   SIZE_T *size, ULONG unknown );
    NTSTATUS      (WINAPI *NtUnmapViewOfSection)( HANDLE process, PVOID addr );
    NTSTATUS      (WINAPI *NtWriteVirtualMemory)( HANDLE process, void *addr, const void *buffer,
                                                  SIZE_T size, SIZE_T *bytes_written );

    /* environment functions */
    void          (CDECL *get_main_args)( int *argc, char **argv[], char **envp[] );
    void          (CDECL *get_paths)( const char **builddir, const char **datadir, const char **configdir );
    void          (CDECL *get_dll_path)( const char ***paths, SIZE_T *maxlen );
    void          (CDECL *get_unix_codepage)( CPTABLEINFO *table );
    const char *  (CDECL *get_version)(void);
    const char *  (CDECL *get_build_id)(void);
    void          (CDECL *get_host_version)( const char **sysname, const char **release );

    /* loader functions */
    NTSTATUS      (CDECL *exec_wineloader)( char **argv, int socketfd, int is_child_64bit,
                                            ULONGLONG res_start, ULONGLONG res_end );

    /* virtual memory functions */
    NTSTATUS      (CDECL *map_so_dll)( const IMAGE_NT_HEADERS *nt_descr, HMODULE module );
    NTSTATUS      (CDECL *virtual_map_section)( HANDLE handle, PVOID *addr_ptr, unsigned short zero_bits_64, SIZE_T commit_size,
                                                const LARGE_INTEGER *offset_ptr, SIZE_T *size_ptr, ULONG alloc_type,
                                                ULONG protect, pe_image_info_t *image_info );
    void          (CDECL *virtual_get_system_info)( SYSTEM_BASIC_INFORMATION *info );
    NTSTATUS      (CDECL *virtual_create_builtin_view)( void *module );
    TEB *         (CDECL *virtual_alloc_first_teb)(void);
    NTSTATUS      (CDECL *virtual_alloc_thread_stack)( INITIAL_TEB *stack, SIZE_T reserve_size, SIZE_T commit_size, SIZE_T *pthread_size );
    NTSTATUS      (CDECL *virtual_handle_fault)( LPCVOID addr, DWORD err, BOOL on_signal_stack );
    unsigned int  (CDECL *virtual_locked_server_call)( void *req_ptr );
    ssize_t       (CDECL *virtual_locked_read)( int fd, void *addr, size_t size );
    ssize_t       (CDECL *virtual_locked_pread)( int fd, void *addr, size_t size, off_t offset );
    ssize_t       (CDECL *virtual_locked_recvmsg)( int fd, struct msghdr *hdr, int flags );
    BOOL          (CDECL *virtual_is_valid_code_address)( const void *addr, SIZE_T size );
    int           (CDECL *virtual_handle_stack_fault)( void *addr );
    BOOL          (CDECL *virtual_check_buffer_for_read)( const void *ptr, SIZE_T size );
    BOOL          (CDECL *virtual_check_buffer_for_write)( void *ptr, SIZE_T size );
    SIZE_T        (CDECL *virtual_uninterrupted_read_memory)( const void *addr, void *buffer, SIZE_T size );
    NTSTATUS      (CDECL *virtual_uninterrupted_write_memory)( void *addr, const void *buffer, SIZE_T size );
    void          (CDECL *virtual_set_force_exec)( BOOL enable );
    void          (CDECL *virtual_release_address_space)(void);
    void          (CDECL *virtual_set_large_address_space)(void);

    /* thread/process functions */
    void          (CDECL *init_threading)( int *nb_threads, struct ldt_copy **ldt_copy );
    NTSTATUS      (CDECL *create_thread)( SIZE_T stack_reserve, SIZE_T stack_commit, HANDLE actctx,
                                          DWORD tid, int request_fd, PRTL_THREAD_START_ROUTINE start,
                                          void *param, void *relay );
    void          (CDECL *start_process)( PRTL_THREAD_START_ROUTINE entry, BOOL suspend, void *relay );
    void          (CDECL *abort_thread)( int status );
    void          (CDECL *exit_thread)( int status );
    void          (CDECL *exit_process)( int status );
    NTSTATUS      (CDECL *get_thread_ldt_entry)( HANDLE handle, void *data, ULONG len, ULONG *ret_len );

    /* server functions */
    unsigned int  (CDECL *server_call)( void *req_ptr );
    unsigned int  (CDECL *server_select)( const select_op_t *select_op, data_size_t size, UINT flags,
                                          timeout_t abs_timeout, CONTEXT *context, RTL_CRITICAL_SECTION *cs,
                                          user_apc_t *user_apc );
    unsigned int  (CDECL *server_wait)( const select_op_t *select_op, data_size_t size, UINT flags,
                                        const LARGE_INTEGER *timeout );
    unsigned int  (CDECL *server_queue_process_apc)( HANDLE process, const apc_call_t *call, apc_result_t *result );
    void          (CDECL *server_send_fd)( int fd );
    int           (CDECL *server_get_unix_fd)( HANDLE handle, unsigned int wanted_access, int *unix_fd,
                                               int *needs_close, enum server_fd_type *type, unsigned int *options );
    NTSTATUS      (CDECL *server_fd_to_handle)( int fd, unsigned int access, unsigned int attributes,
                                                HANDLE *handle );
    NTSTATUS      (CDECL *server_handle_to_fd)( HANDLE handle, unsigned int access, int *unix_fd,
                                                unsigned int *options );
    void          (CDECL *server_release_fd)( HANDLE handle, int unix_fd );
    int           (CDECL *server_pipe)( int fd[2] );
    void          (CDECL *server_init_process)(void);
    void          (CDECL *server_init_process_done)(void);
    size_t        (CDECL *server_init_thread)( void *entry_point, BOOL *suspend, unsigned int *cpus,
                                               BOOL *wow64, timeout_t *start_time );

    /* debugging functions */
    void          (CDECL *dbg_init)(void);
    unsigned char (CDECL *dbg_get_channel_flags)( struct __wine_debug_channel *channel );
    const char *  (CDECL *dbg_strdup)( const char *str );
    int           (CDECL *dbg_output)( const char *str );
    int           (CDECL *dbg_header)( enum __wine_debug_class cls, struct __wine_debug_channel *channel,
                                       const char *function );
};

#endif /* __NTDLL_UNIXLIB_H */