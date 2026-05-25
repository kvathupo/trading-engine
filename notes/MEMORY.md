# Virtual Address Translation in Linux
The kernel operates in Ring 0 with direct hardware access at the expense of 
unhandled errors crashing the whole system. Processes operate in Ring 3 with
access to virtual memory addresses, which are translated by the kernel.

Hardware reads and writes to a __translation lookaside buffer__ that maintains
virtual page address to _physical frame address_ mappings. Physical frame 
addresses point to locations in RAM, which can be resolved by the Kernel to 
a file anywhere in the memory hierarchy, I/O device, etc.

Hardware reads from a  __page table__, which is written to by the kernel.
It is unique to each process. It is composed of page table entries. A page
table entry translates a virtual memory page address to a physical memory 
frame address. Physical memory frames are maintained by the OS, and exist in RAM.

The kernel maintains a __page cache__ that is shared across all processes on 
the software level. Page cache entries are blocks of file data existing on RAM, 
copied from a part of a file, existing somewhere in the memory hierarchy. They
are typically 4 KiB, which is around the size of a page table entry at 4KB. All
files in linux have an inode, which is filesystem metadata dictating access and 
location. Whenever a file is accessed, kernel space code maintains a list of 
open file descriptions holding a file offset (a position to the current read/write
position for a file descriptor struct, returned by `open()`). These file descriptions
are then returned to user-space programs openning files. Hence there's overhead from the CPU
switching from user space code requesting opens/reads to kernel space code maintaining 
the file descriptions.

## Memory Access
First, a `struct file` file descriptor is returned by an `open()` sys call. This simply
sets the current read/write position, and allocates a ref-counted refrence to handle on 
the kernel's open file table. This object on the kernel's open file table is only released
when refs go to zero. No memory is moved up or down the memory hierarchy until a read/write.
This is on-demand paging.

When accessing memory referred to by a file descriptor, the virtual page address serves an
input to the TLB, which will miss lookup. Thus, a page walk is done on the page table. If an
entry for that virtual page address is not found, this is a SIGSEGV in user space and fault in
kernel space. In the latter case, we have a kernel oops or kernel panic. If found (the case for
correct `mmap()` code a `read()` syscall), then it will have the Present bit set to 0. This raises
a page fault that triggers kernel space code. 

With this fault, in the case of files, the kernel checks the page cache. On a hit, it reuases an 
existing folio in a __minor fault__. In a miss (__major page fault__), a page cache entry is populated 
from lower in the memory hierarchy, and copied up into kernel space memory in RAM. Recall the file 
decriptor holds an offset informing _where_ in the file to start reads/writes. The kernel writes
a page table entry to the page table. This entry has a physical frame address pointing to this page
cache entry. Recall physical frame addresses _must_ exist on RAM; hence page caches existing on RAM!

After writing a page table entry, the kernel returns to the fault handler callback. The CPU repeats
the faulting instruction. It checks the TLB to find no entry mapping to a physical frame address.
A secondpage walk occurs. Now, it finds a page table entry with the Present bit set. The 

## MMap vs open()
It is expensive for the CPU to context switch between kernel space and user space code. The only
performance advantage of `mmap()` is that it skips kernel code writing from kernel space page cache
entries to user space buffers referring to file contents. Instead, a virtual memory address is created
in user space code, and a page fault is triggered in user-space code in order to populate the TLB. Cons
of Mmap:
1. With a major page fault, the thread incurs a synchronrous I/O stall. With `read()` or `io_uring`, you 
get back control with `errno` set.
2. I/O errors become a SIGBUS, e.g. file truncation or general read error.
3. Page tables are aligned to 4KB. If files are smaller than this, you will fragment the page tabe in the 
best case. In the worst case, you always thrash the TLB.

Pros of Mmap:
1. If you need to do many `pread()` operations (`read()` syscalls with an offset). 
2. If you want to share read-only data structures across many processes.
