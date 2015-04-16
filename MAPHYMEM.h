/* MAPHYMEM.h - DosMapPhysMem() dynalink library, public release 1.0.1 (build 2)
     Copyright (c) 2001 Takayuki 'January June' Suwa

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA. */

#if !defined(__MAPHYMEM)
#define __MAPHYMEM

#if defined(__cplusplus)
extern "C" {
#endif

/* Maps specified physical memory region to 32bit process private address space
   return : !NO_ERROR -> error occurred
            NO_ERROR  -> successful completion */
extern APIRET APIENTRY DosMapPhysMem(PPVOID ppb,       /* (output) pointer to variable that, pointer to mapped memory region will be set to */
                                     ULONG cb,         /* (input)  size of memory region to map, in bytes */
                                     ULONG ulPhysAddr  /* (input)  physical memory address to be mapped */);

/* Maps specified physical memory region to 32bit process shared address space
   return : !NO_ERROR -> error occurred
            NO_ERROR  -> successful completion */
extern APIRET APIENTRY DosMapSharedPhysMem(PPVOID ppb,       /* (output) pointer to variable that, pointer to mapped memory region will be set to */
                                           ULONG cb,         /* (input)  size of memory region to map, in bytes */
                                           ULONG ulPhysAddr  /* (input)  physical memory address to be mapped */);

/* Unmaps physical memory region mapped by DosMap*PhysMem() from 32bit process address space
   return : !NO_ERROR -> error occurred
            NO_ERROR  -> successful completion */
extern APIRET APIENTRY DosUnmapPhysMem(PVOID pb  /* (input) pointer to mapped memory region to be unmapped */);

#if defined(__cplusplus)
}
#endif

#endif

