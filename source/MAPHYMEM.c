/* MAPHYMEM.c - DosMapPhysMem() dynalink library, public release 1.0.1 (build 1)
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

#pragma strings(readonly)
#pragma entry(dllentry)
#define INCL_BASE
#define INCL_DOSDEVIOCTL
#define INCL_ERRORS
#include <os2.h>
#include "MAPHYMEM.h"

/* internal per-process variables */
static HFILE G_hfSCREENDD;  /* file handle of SCREENDD, \DEV\SCREEN$ */

/* 32bit OS/2 DLL initialization/termination entrypoint */
ULONG APIENTRY dllentry(HMODULE hmodLibrary,
                        BOOL bTerminate)
{
    ULONG ulWork;
    if(bTerminate == (BOOL)FALSE)
    {
        ulWork = (ULONG)1;
        (VOID)DosSetRelMaxFH((PLONG)&ulWork,
                             &ulWork);
        if(DosOpen((PSZ)"\\DEV\\SCREEN$",
                   &G_hfSCREENDD,
                   &ulWork,
                   (ULONG)0,
                   FILE_NORMAL,
                   OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
                   OPEN_FLAGS_FAIL_ON_ERROR | OPEN_FLAGS_NOINHERIT | OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY,
                   (PEAOP2)NULL) == NO_ERROR)
        {
            /* function existence checking
               if not supported, ERROR_I24_BAD_COMMAND will be returned */
            if(DosDevIOCtl(G_hfSCREENDD,
                           SCREENDD_CATEGORY,
                           SCREENDD_GETLINEARACCESS,
                           (PVOID)NULL,
                           (ULONG)0,
                           (PULONG)NULL,
                           (PVOID)NULL,
                           (ULONG)0,
                           (PULONG)NULL) != (APIRET)0xff03 &&  /* ERROR_I24_BAD_COMMAND */
               DosDevIOCtl(G_hfSCREENDD,
                           SCREENDD_CATEGORY,
                           SCREENDD_FREEGLOBALACCESS,
                           (PVOID)NULL,
                           (ULONG)0,
                           (PULONG)NULL,
                           (PVOID)NULL,
                           (ULONG)0,
                           (PULONG)NULL) != (APIRET)0xff03)  /* ERROR_I24_BAD_COMMAND */
                goto passed;
            (VOID)DosClose(G_hfSCREENDD);
            G_hfSCREENDD = (HFILE)NULLHANDLE;
        }
        ulWork = (ULONG)-1;
        (VOID)DosSetRelMaxFH((PLONG)&ulWork,
                             &ulWork);
passed:;
    }
    else
    {
        if(G_hfSCREENDD != (HFILE)NULLHANDLE)
        {
            (VOID)DosClose(G_hfSCREENDD);
            ulWork = (ULONG)-1;
            (VOID)DosSetRelMaxFH((PLONG)&ulWork,
                                 &ulWork);
        }
    }
    return (ULONG)-1;
}

/* Maps specified physical memory region to 32bit process private address space */
APIRET DosMapPhysMem(PPVOID ppb,
                     ULONG cb,
                     ULONG ulPhysAddr)
{
    APIRET rc = ERROR_INVALID_FUNCTION;
    GETLINIOCTLDATA xParm;
    ULONG ulParmLen;
    if(G_hfSCREENDD != (HFILE)NULLHANDLE)
    {
        xParm.PacketLength = (ULONG)sizeof(GETLINIOCTLDATA);
        xParm.PhysicalAddress = ulPhysAddr;
        if(ulPhysAddr == (ULONG)0)
            xParm.PhysicalAddress = (ULONG)1;
        xParm.ApertureSize = cb;
        xParm.LinearFlags = GETLINEAR_FLAG_MAPPHYSICAL | GETLINEAR_FLAG_MAPPROCESS;
        rc = DosDevIOCtl(G_hfSCREENDD,
                         SCREENDD_CATEGORY,
                         SCREENDD_GETLINEARACCESS,
                         (PVOID)&xParm,
                         ulParmLen = (ULONG)sizeof(xParm),
                         &ulParmLen,
                         (PVOID)NULL,
                         (ULONG)0,
                         (PULONG)NULL);
        if(rc == NO_ERROR)
            *ppb = (PVOID)xParm.LinearAddress;
        else
            rc = ERROR_INVALID_PARAMETER;
    }
    return rc;
}

/* Maps specified physical memory region to 32bit process shared address space */
APIRET DosMapSharedPhysMem(PPVOID ppb,
                           ULONG cb,
                           ULONG ulPhysAddr)
{
    APIRET rc = ERROR_INVALID_FUNCTION;
    GETLINIOCTLDATA xParm;
    ULONG ulParmLen;
    if(G_hfSCREENDD != (HFILE)NULLHANDLE)
    {
        xParm.PacketLength = (ULONG)sizeof(GETLINIOCTLDATA);
        xParm.PhysicalAddress = ulPhysAddr;
        if(ulPhysAddr == (ULONG)0)
            xParm.PhysicalAddress = (ULONG)1;
        xParm.ApertureSize = cb;
        xParm.LinearFlags = GETLINEAR_FLAG_MAPPHYSICAL | GETLINEAR_FLAG_MAPSHARED;
        rc = DosDevIOCtl(G_hfSCREENDD,
                         SCREENDD_CATEGORY,
                         SCREENDD_GETLINEARACCESS,
                         (PVOID)&xParm,
                         ulParmLen = (ULONG)sizeof(xParm),
                         &ulParmLen,
                         (PVOID)NULL,
                         (ULONG)0,
                         (PULONG)NULL);
        if(rc == NO_ERROR)
            *ppb = (PVOID)xParm.LinearAddress;
        else
            rc = ERROR_INVALID_PARAMETER;
    }
    return rc;
}

/* Unmaps physical memory region mapped by DosMap*PhysMem() from 32bit process address space */
APIRET DosUnmapPhysMem(PVOID pb)
{
    APIRET rc = ERROR_INVALID_FUNCTION;
    GETGLOBALPACKET xParm;
    ULONG ulParmLen;
    if(G_hfSCREENDD != (HFILE)NULLHANDLE)
    {
        xParm.GlobalPktLength = (ULONG)sizeof(GETGLOBALPACKET);
        xParm.GlobalPktData[0].GlobalAddress = (ULONG)pb;
        rc = DosDevIOCtl(G_hfSCREENDD,
                         SCREENDD_CATEGORY,
                         SCREENDD_FREEGLOBALACCESS,
                         (PVOID)&xParm,
                         ulParmLen = (ULONG)sizeof(xParm),
                         &ulParmLen,
                         (PVOID)NULL,
                         (ULONG)0,
                         (PULONG)NULL);
        if(rc != NO_ERROR)
            rc = ERROR_INVALID_PARAMETER;
    }
    return rc;
}

