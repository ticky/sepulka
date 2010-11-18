/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *          ____   ________   ________                                    ________       *
 *         |      |          |        | |        | |          |        | |        |      *
 *         |      |________  |________| |        | |          |________| |________|      *
 *         |      |          |          |        | |          |       \  |        |      *
 *     ____|      |________  |          |________| |________  |        \ |        |      *
 *                                                                                       *
 *                                                                     by C4TurD4Y       *
 *                                                                                       *
 *  Sepulka is another selective plugin loader for PlayStation Portable. From 0.3c it's  *
 *  an open source project.                                                              *
 *                                                                                       *
 *   Copyright (C) 2011 C4TurD4Y                                                         *
 *                                                                                       *
 *   This program is free software: you can redistribute it and/or modify                *
 *   it under the terms of the GNU General Public License as published by                *
 *   the Free Software Foundation, either version 3 of the License, or                   *
 *   (at your option) any later version.                                                 *
 *                                                                                       *
 *   This program is distributed in the hope that it will be useful,                     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of                      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                       *
 *   GNU General Public License for more details.                                        *
 *                                                                                       *
 *   You should have received a copy of the GNU General Public License                   *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.               *
 *                                                                                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                       *
 *   Special macros (useful for logging):                                                                     *
 *   ENABLE_LOGGING     - Enable logging                                                 *
 *   ENABLE_LOGGING_ALL - Log all events                                                 *
 *   LOG_TO_SCREEN      - Enable logging to screen                                       *
 *   LOG_TO_FILE        - Enable logging to file                                         *
 *                                                                                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem.h>
#include <stdio.h>
#include <pspiofilemgr.h>
#include <pspumd.h>
#include <psppower.h>
#include <pspdisplay.h>
#include <pspdisplay_kernel.h>
#include <psploadexec_kernel.h>
#include <pspdebug.h>
#include <psprtc.h>
#include "apihook.h"
#include "systemctrl_se_lite.h"

//#define PSP_GO_SUPPORT

#define LOG_TO_SCREEN
#define LOG_TO_FILE
#define ENABLE_LOGGING
//#define ENABLE_LOGGING_ALL //Must be defined after ENABLE_LOGGING

#define TITLE_STRING "Sepulka v0.7c [PGC Edition] by C4TurD4Y"
#define INFO_STRING  "Check out PSP Genesis Competition 2011 at http://wololo.net/genesis/"

#define runSepulkaTxt() runModuleList("ms0:/seplugins/sepulka.txt", 0)

#define RML_NORMAL 0
#define RML_CUSTOM 1
#define RML_ISODRV 2
#define RML_GAMPRO 3
#define RML_MACROS 4
#define RML_SETTIN 6

int patchClock();
int unpatchClock();

PSP_MODULE_INFO("sepulka", 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

int sceSysreg_driver_AB3185FD(float cpuclock); //CPU changing
int sceSysreg_driver_136E8F5A(float busclock); //Bus changing

enum GameModes
{
    unkMode = 0x0,
    isoMode = 0x1,
    umdMode = 0x2,
    pbpMode = 0x3,
};

char * isodrvs[] = {
                      "unknown",
                      "normal",
                      "m33driver",
                      "np9660"
                   };

char * modes[] = {
                    "unknown", // unknown
                    "iso", // ISO game
                    "umd", // UMD game
                    "pbp" // Memory Stick games (PSN & Homebrew)
                 };

char gameid[16];
char gametitle[256];
int decisloaded = 0;
int isodrv;
int mode;
char numbers[] = "0123456789";

typedef struct
{
    char   name[256];
    char   macro[256];
} Macro;

Macro macros[256];
int macros_num = 0;

char path_to_file[4096];

char * onoff_str[] = {"on", "off"};
char * setting_screenlog_str = "bootscreen";
char * setting_filelog_str   = "filelog";
char * setting_autosort_str  = "autosort";

char disable_autosort = 0;

#ifdef ENABLE_LOGGING

SceUID logd;

#ifdef LOG_TO_SCREEN
char disable_screenlog = 1;
#endif

#ifdef LOG_TO_FILE
char disable_filelog = 1;
#endif

int log_inited = 0;

void logInit()
{
    if (log_inited == 0)
    {

#ifdef LOG_TO_FILE
#ifndef ENABLE_LOGGING_ALL
        if (!disable_filelog)
        {
#endif
            logd = sceIoOpen("ms0:/log.txt", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_APPEND, 0777);
#ifndef ENABLE_LOGGING_ALL
        }
#endif
#endif

#ifdef LOG_TO_SCREEN
#ifndef ENABLE_LOGGING_ALL
        if (!disable_screenlog)
        {
#endif
            unsigned int a_address=0;
            unsigned int a_bufferWidth=0;
            unsigned int a_pixelFormat=0;
            unsigned int a_sync;

            pspDebugScreenInitEx(0x44000000, 0, 1);

            sceDisplayGetMode(&a_pixelFormat, &a_bufferWidth, &a_bufferWidth);
            pspDebugScreenSetColorMode(a_pixelFormat);
            pspDebugScreenSetTextColor(0xFFFFFFFF);

            sceDisplayWaitVblankStart();
#ifndef ENABLE_LOGGING_ALL
        }
#endif

#endif

    log_inited = 1;

    char text[256];
    logWrite(TITLE_STRING);
    logWrite(INFO_STRING);
    sprintf(text, "Mode: %s", modes[mode]);
    logWrite(text);
    if (mode == isoMode)
    {
        sprintf(text, "Iso driver: %s", isodrvs[isodrv]);
        logWrite(text);
    }
    sprintf(text, "Game ID: %s", gameid);
    logWrite(text);
    sprintf(text, "Game title: %s", gametitle);
    logWrite(text);
    logWrite(" ");

    }
    else
    {
#ifdef LOG_TO_FILE
        sceIoWrite(logd, "\x0d\x0a", 2);
        sceIoClose(logd);
#endif
    log_inited = 0;
/*
#ifdef LOG_TO_SCREEN

#endif*/
    }

}

int logWrite(const char * text)
{
    //if (log_inited == 0) logInit();
    if (log_inited == 1)
    {
#ifdef LOG_TO_FILE
#ifndef ENABLE_LOGGING_ALL
        if (!disable_filelog)
        {
#endif
            pspTime time;
            char data[64];
            sceRtcGetCurrentClockLocalTime(&time);
            sprintf(data,"[%.4d/%.2d/%.2d %.2d:%.2d:%.2d] ",time.year,time.month,time.day,time.hour,time.minutes,time.seconds);

            sceIoWrite(logd, data, strlen(data));
            sceIoWrite(logd, text, strlen(text));
            sceIoWrite(logd, "\x0d\x0a", 2);
#ifndef ENABLE_LOGGING_ALL
        }
#endif
#endif

#ifdef LOG_TO_SCREEN
#ifndef ENABLE_LOGGING_ALL
        if (!disable_screenlog)
        {
#endif
            pspDebugScreenKprintf(text);
            pspDebugScreenKprintf("\n");
#ifndef ENABLE_LOGGING_ALL
        }
#endif
#endif
    }

    return 0;
}

#endif

int patchedUmdDeactivate (int unit, const char * drive) {return 0;}
void * oldUmdDeactivate;
u32 * syscall;

int patchUmd()
{
    SceModule * pMod = sceKernelFindModuleByName("sceUmd_driver"); // Normal driver
    if (pMod == NULL)
    {
        pMod = sceKernelFindModuleByName("sceNp9660_driver"); // Sony NP9660 driver
        if (pMod == NULL)
        {
            pMod = sceKernelFindModuleByName("pspMarch33_Driver"); // M33 driver
            if (pMod == NULL)
            {
                return -1;
            }
        }
    }
    oldUmdDeactivate = libsFindExportByNid(pMod->modid, "sceUmd", 0xE83742BA);
    syscall = find_syscall_addr(oldUmdDeactivate);
    if(!oldUmdDeactivate) return -2;
    if(apiHookByNid(pMod->modid, "sceUmd",0xE83742BA,patchedUmdDeactivate) == 0) return -3;
    return 0;
}

int unpatchUmd()
{
    if (oldUmdDeactivate == 0) return -1;
    int intc = pspSdkDisableInterrupts();
    *syscall = oldUmdDeactivate;
    oldUmdDeactivate = 0;
    sceKernelDcacheWritebackInvalidateRange(syscall, sizeof(void *));
    sceKernelIcacheInvalidateRange(syscall, sizeof(void *));
    pspSdkEnableInterrupts(intc);
    return 0;
}


//Thanks Zer01ne and Black dev team for incomplete systemctrl.prx
#define MAKE_DUMMY_FUNCTION0(a) _sw(0x03E00008, a); _sw(0x00001021, a+4)

int patchClock()
{
    int k1 = pspSdkSetK1(0);

    SceModule * pMod = sceKernelFindModuleByName("scePower_Service"); //power.prx
    if (pMod == NULL)
    {
        return -1;
    }

    void * hookFunc;

    hookFunc = libsFindExportByNid(pMod->modid, "scePower", 0x737486F2); //scePowerSetClockFrequency
   MAKE_DUMMY_FUNCTION0((u32)hookFunc);
    //else logWrite("Error 1");

    hookFunc = libsFindExportByNid(pMod->modid, "scePower", 0x843FBF43); //scePowerSetCpuClockFrequency
   MAKE_DUMMY_FUNCTION0((u32)hookFunc);
    //else logWrite("Error 2");

    hookFunc = libsFindExportByNid(pMod->modid, "scePower", 0xB8D7B3FB); //scePowerSetBusClockFrequency
   MAKE_DUMMY_FUNCTION0((u32)hookFunc);
    //else logWrite("Error 3");

    sceKernelIcacheInvalidateAll();
	sceKernelDcacheWritebackInvalidateAll();

    hookFunc = libsFindExportByNid(pMod->modid, "scePower", 0x545A7F3C); //scePowerSetClockFrequency mirror (POPS)
    MAKE_DUMMY_FUNCTION0((u32)hookFunc);
    //else logWrite("Error 4");

    hookFunc = libsFindExportByNid(pMod->modid, "scePower", 0xA4E93389); //scePowerSetClockFrequency mirror
    MAKE_DUMMY_FUNCTION0((u32)hookFunc);
    //else logWrite("Error 5");

    hookFunc = libsFindExportByNid(pMod->modid, "scePower", 0xEBD177D6); //scePowerSetClockFrequency mirror
     MAKE_DUMMY_FUNCTION0((u32)hookFunc);
    //else logWrite("Error 6");

	sceKernelIcacheInvalidateAll();
	sceKernelDcacheWritebackInvalidateAll();

	pspSdkSetK1(k1);


    return 0;
}

int changeClock(int cpu_clock)
{
    int bus_clock;

   /*if (cpu_clock <= 20) cpu_clock = 20;
    else /if(cpu_clock <= 85) {cpu_clock = 85;}
        else */if(cpu_clock <= 100) cpu_clock = 100;
            else if(cpu_clock <= 133) cpu_clock = 133;
                else if(cpu_clock <= 222) cpu_clock = 222;
                    else if(cpu_clock <= 266) cpu_clock = 266;
                        else if(cpu_clock <= 300) cpu_clock = 300;
                            else cpu_clock = 333;

    bus_clock = (int)cpu_clock/2;

    scePowerSetClockFrequency(cpu_clock, cpu_clock, bus_clock);

    sceSysreg_driver_AB3185FD(cpu_clock); //CPU changing
    sceSysreg_driver_136E8F5A(bus_clock); //Bus changing

    //scePowerSetCpuClockFrequency(cpu_clock);
    //scePowerSetBusClockFrequency(bus_clock);

#ifdef ENABLE_LOGGING
#ifdef ENABLE_LOGGING_ALL
    char text[256];
    sprintf(text, "CPU: %d Bus: %d", scePowerGetCpuClockFrequency(), scePowerGetBusClockFrequency());
    logWrite(text);
    logWrite(" ");
#endif
#endif

    return 0;
}

int changeTimeStamp()
{
    if (disable_autosort) return 1;
    SceIoStat stat;
    pspTime time;
    char * temp;
    memset(&stat, 0, sizeof(stat));
    memset(&time, 0, sizeof(stat));
    sceRtcGetCurrentClockLocalTime(&time);

    if ((strcmp(modes[mode], "umd") != 0) && (strcmp(modes[mode], "unknown") != 0))
    {
        if (!strcmp(modes[mode], "pbp"))
        {
            temp = strrchr(path_to_file ,'/');
            temp[0] = '\0';
        }

        sceIoGetstat(path_to_file, &stat);
        stat.st_mtime.year = time.year;
        stat.st_mtime.month = time.month;
        stat.st_mtime.day = time.day;
        stat.st_mtime.hour = time.hour;
        stat.st_mtime.minute = time.minutes;
        stat.st_mtime.second = time.seconds;
        stat.st_mtime.microsecond = time.microseconds;
        //For backups
        stat.st_ctime.year = time.year;
        stat.st_ctime.month = time.month;
        stat.st_ctime.day = time.day;
        stat.st_ctime.hour = time.hour;
        stat.st_ctime.minute = time.minutes;
        stat.st_ctime.second = time.seconds;
        stat.st_ctime.microsecond = time.microseconds;
        sceIoChstat(path_to_file, &stat, 0xFFFFFFFF);
    }
    return 0;
}

int getIsodrv()
{
    SceModule * pMod = sceKernelFindModuleByName("sceUmd_driver"); // Normal driver
    if (pMod == NULL)
    {
        pMod = sceKernelFindModuleByName("sceNp9660_driver"); // Sony NP9660 driver
        if (pMod == NULL)
        {
            pMod = sceKernelFindModuleByName("pspMarch33_Driver"); // M33 driver
            if (pMod == NULL)
            {
                return 0;
            }
            else return 2;
        }
        else return 3;
    }
    else return 1;
}

//It's a part of my unreleased Recovery Menu (ended in 99%, could be successfully used as DaX' RM replacement)

typedef struct
{
    u32 version;       //Should be 0x01010000
    u32 keyTable;      //Offset of Key Table
    u32 valueTable;    //Offset of Value Table
    u32 itemsNum;      //Number of Data Items
} ParamSfo;

typedef struct
{
    u16 nameOffset;    //Offset of Key Name in Key Table
    u8  dataAlignment; //Always 04
    u8  valueType;     //Data type of value (0 - binary; 2 - text; 4 - Number (32))
    u32 valueSize;     //Size of Value data
    u32 valueSizeWithPadding; //Size of Value data and Padding
    u32 valueOffset;   //Offset of Data Value in Data Table	;
} Key;

int psfGetKey(const char * keyName, const char * data, char * value)
{
    int seek = 0;
    int foo = 0;

    ParamSfo info;
    memset(&info, 0, sizeof(info));
    Key key;
    memset(&key, 0, sizeof(key));

    //if (pspPSFCheck(data) == -1) return -1;
    seek = 4;
    memcpy(&info, &data[4], sizeof(info));
    seek += sizeof(info);
    for (foo = 0; foo < info.itemsNum; foo++)
    {
        memcpy(&key, &data[seek], sizeof(key));
        if (!strcmp(&data[info.keyTable+key.nameOffset], keyName))
        {
            memcpy(value, &data[info.valueTable+key.valueOffset], key.valueSize);
            return key.valueType;
        }
        seek += sizeof(key);
    }
    return -2;
}

void launchIso(int pmode)
{
    struct SceKernelLoadExecVSHParam param;
    memset(&param, 0, sizeof(param));
    param.size = sizeof(param);
    param.args = strlen("disc0:/PSP_GAME/SYSDIR/EBOOT.BIN")+1;
    param.argp = "disc0:/PSP_GAME/SYSDIR/EBOOT.BIN";
    param.key = "game";

    int foo = 0;

    int k1;
/*
    SEConfig config;

    memset(&config, 0, sizeof(config));
    sctrlSEGetConfigEx(&config, sizeof(config));*/

    switch (pmode)
    {
        case 0:/*
                k1 = pspSdkSetK1(0);
                sctrlSESetBootConfFileIndex(0);
                pspSdkSetK1(k1);
            break;*/

        case 1:/*
                k1 = pspSdkSetK1(0);
                sctrlSESetBootConfFileIndex(1);
                pspSdkSetK1(k1);
            break;*/

        case 2:
                k1 = pspSdkSetK1(0);
                sctrlSESetBootConfFileIndex(pmode);
                pspSdkSetK1(k1);
            break;

        default:
            return;

        /*case MODE_OE_LEGACY:
            foo = 1;
            break;*/
    }

    /*if (foo)
    {
        sctrlSEMountUmdFromFile(path, 1, 1);
        return;
    }*/


    k1 = pspSdkSetK1(0);
    sceUmdDeactivate(1, "disc0:");
    sctrlSEUmountUmd();
    sctrlSESetDiscOut(1);
    sctrlSESetDiscOut(0);
    sctrlSESetUmdFile(path_to_file);
    pspSdkSetK1(k1);

    sctrlKernelLoadExecVSHWithApitype(0x120, "disc0:/PSP_GAME/SYSDIR/EBOOT.BIN", &param);

    sceKernelSelfStopUnloadModule(1,0,0);
    sceKernelExitDeleteThread(0);
}

SceUID runPrx(const char * path)
{
    int status;
    SceUID modid = sceKernelLoadModule(path, 0, NULL);
    modid = sceKernelStartModule(modid, (strlen(path) + 1), (void *)path, &status, NULL);
#ifdef ENABLE_LOGGING
    char text[256];
    if (log_inited == 0) logInit();
    if (modid < 0x80000000) sprintf(text, "%s loaded successfully.", strrchr(path,'/')+1);
    else sprintf(text, "Couldn\'t load %s module. Error 0x%08X", strrchr(path,'/')+1, modid);
    logWrite(text);
#endif
    return modid;

}


int makeSmall(char * str)
{
    int changed = 0;
    int foo = strlen(str);
    int bar = 0;
    for (bar = 0; bar < foo; bar++)
    {
        if ((str[bar] >= 0x41) && (str[bar] <= 0x5A))
        {
            str[bar] += 0x20;
            changed++;
        }
    }
    return changed;
}

int knownSignsOnly(char * str)
{
    //char c = 0;
    int foo = 0;
    int shift = 0;
    int bar = strlen(str);
    int spam = 1;
    //int bar = 0;
    for(;foo+shift < bar; foo++)
    {
        //a-z; A-Z; 0-9; |; ~; (; );
        while (!(((str[foo+shift] >= 0x30) && (str[foo+shift] <= 0x39)) || ((str[foo+shift] >= 0x41) && (str[foo+shift] <= 0x5A)) || ((str[foo+shift] >= 0x61) && (str[foo+shift] <= 0x7A)) || (str[foo+shift] == 0x7E) || (str[foo+shift] == 0x28) || (str[foo+shift] == 0x29)) && (foo+shift < bar))
        {
            if (foo+shift == bar-1)
            {
                memset(str+foo, 0, bar-foo);
                return spam;
            }

            str[foo+shift] = 0;
            shift++;
        }
        //if (str[foo+shift] == 0) break;
        //if (str[foo+shift] == 0x7C) spam++;
        str[foo] = str[foo+shift];
       // if (shift > 0) str[foo+shift] = 0;
    }
    memset(str+foo, 0, bar-foo);
    return spam;
}

int getArgsNum(char * str)
{
    int foo = 0;
    int bar = strlen(str);
    int spam = 1;
    for(;foo < bar; foo++)
    {
        if (str[foo] == 0x7C) spam++;
    }
    return spam;
}


int runPlugin(const char * path, char * attrs, int pmode)
{
    int foo;
    foo = getArgsNum(attrs);
    makeSmall(attrs);
    int bar = 0;
    char arg[256];
    memset(arg, 0, sizeof(arg));
    char * pch=attrs;
    char * pch2;
    int arglen = 0;
    int spam = 0;
    char * cycki = (char *)path;

    SceUID modid;

//#ifdef ENABLE_LOGGING
    char text[64];
//#endif

    int banan = 0;

    for(bar = 0;bar < foo;bar++)
    {
        memset(arg, 0, sizeof(arg));
        pch2 = strchr(pch+1, 0x7C);
        if (pch2 != NULL) arglen = pch2-pch;
        else (arglen = strlen(attrs)-(pch-attrs));
        memcpy(arg, pch, arglen);
        pch = pch2+1;
        spam = checkPlugin(arg);
        if (spam == 1)
        {
            banan = 1;
        }
        if (spam == -1)
        {
            banan = 0;
            break;
        }
    }

    if (banan == 1)
    {
        if (pmode == RML_ISODRV)
        {
            makeSmall(cycki);
            knownSignsOnly(cycki);
            if (!strcmp(isodrvs[isodrv], cycki)) return -2;
            if (strcmp("normal", cycki) != 0 && strcmp("m33driver", cycki) != 0 && strcmp("np9660", cycki) != 0) return -1;
#ifdef ENABLE_LOGGING
            sprintf(text, "Changing iso driver to %s...", cycki);
            if (log_inited == 0) logInit();
            logWrite(text);
            sceKernelDelayThread(1*1000*1000);
#endif
            if (!strcmp("normal", cycki)) launchIso(0);
            if (!strcmp("m33driver", cycki)) launchIso(1);
            if (!strcmp("np9660", cycki)) launchIso(2);
        }

        else
        {if (pmode == RML_GAMPRO)
        {
            //logWrite("weszlem1");
            makeSmall(cycki);
            knownSignsOnly(cycki);
            if (!memcmp("cpu",cycki,3))
            {
                if(strcspn(cycki+3, numbers) == 0){if(strspn(cycki+3, numbers) > 0)
                {
                    //logWrite("weszlem2");

                    bar = 0;
                    bar += (cycki[3]-0x30)*100;
                    bar += (cycki[4]-0x30)*10;
                    bar += (cycki[5]-0x30);

                    //modid = sceKernelLoadModule("ms0:/seplugins/sepulka_gp.prx", 0, NULL);
                    //modid = sceKernelStartModule(modid, 0, NULL, &spam, NULL);

                    changeClock(bar);
                    patchClock();
                }}
            }
        }

        else

        {
            if (runPrx(path) > 0x80000000) banan = -2;
        }
        }
    };
    return banan;
}

int getLine(SceUID fd, char * str, int max_len)
{
    int i = 0, bytes;
    while( i < max_len && ( bytes = sceIoRead( fd, str + i, 1 ) ) == 1 )
    {
        if ( str[i] == -1 || str[i] == 0x0d || str[i] == 0x0a )
        {
            str[i] = 0;
            break;
        }
        //buf[i]=tolower(buf[i]);
        i ++;
        str[i] = 0;
    }
    if ( bytes != 1 && i == 0 ) return -1;
    return i;
}

int runModuleList(const char * path, int pmode)
{
    SceIoStat fstat;
    char * temp = path;
#ifdef PSP_GO_SUPPORT
    if(pmode != RML_CUSTOM)
    {
        if(!strncmp(path, "ms0:", 4))
        {
            memcpy(temp, "ef0:", 4);
            memset(&fstat, 0, sizeof(SceIoStat));
            sceIoGetstat(path, &fstat);
            if (fstat.st_size <= 0) memcpy(temp, "ms0:", 4);
        }
    }
#endif
    char line[4096];
    char args[2048];
    char ppath[2048];
#ifdef ENABLE_LOGGING
#ifdef ENABLE_LOGGING_ALL
    sprintf(line, "Analizing %s file...", path);
    logWrite(line);
#endif
#endif
    memset(&fstat, 0, sizeof(SceIoStat));
    sceIoGetstat(path, &fstat);
    if (fstat.st_size <= 0) return -1;
    SceUID fd = sceIoOpen(path, PSP_O_RDONLY, 0777);
    if (!fd) return -2;

    char c;
    int spam;

    char * pch;
    char * pch2;

    char mud = 0;
    int x = 0;
    int loaded = 0;

    while (sceIoRead(fd, &c, 1) != 0)
    {
        if (c == 0x0d || c == 0x0a || c == 0x20) continue;
        if (c == 0x23) {mud = 1; continue;}
        else sceIoLseek(fd, -1, SEEK_CUR);
        memset(line, 0, sizeof(line));
        memset(ppath, 0, sizeof(ppath));
        memset(args, 0, sizeof(args));
        getLine(fd, line, 4096);
        spam = 0;

        pch = line;

        if (mud == 0)
        {
            if (pmode != RML_CUSTOM)
            {
                while (1)
                {
                    for(; pch[0] == 0x20; pch++);
                    pch2 = strchr(pch+1, 0x20);
                    if (pch2 - pch > 1) break;
                    if (pch2 == NULL) break;
                }

                if (pch2 - pch <= 0) continue;

                strncpy(ppath, pch, pch2 - pch);
                while(pch2[0] == 0x20) pch2++;
                strncpy(args,  pch2, strlen(pch)-(pch2 - pch));

                switch (pmode)
                {
                    case RML_ISODRV:
                    case RML_GAMPRO:
                        runPlugin(ppath, args, pmode);
                        break;
                    case RML_SETTIN:
                        makeSmall(ppath);
                        makeSmall(args);
                        knownSignsOnly(ppath);
                        knownSignsOnly(args);
//     Settings!       \\
///////////////////////// >
//                     //
#ifdef LOG_TO_SCREEN
                        if ((!strcmp(ppath, setting_screenlog_str)) && (!strcmp(args, onoff_str[0]))) disable_screenlog = 0;
                        if ((!strcmp(ppath, setting_screenlog_str)) && (!strcmp(args, onoff_str[1]))) disable_screenlog = 1;
#endif
#ifdef LOG_TO_FILE
                        if ((!strcmp(ppath, setting_filelog_str)) && (!strcmp(args, onoff_str[0]))) disable_filelog = 0;
                        if ((!strcmp(ppath, setting_filelog_str)) && (!strcmp(args, onoff_str[1]))) disable_filelog = 1;
#endif
                        if ((!strcmp(ppath, setting_autosort_str)) && (!strcmp(args, onoff_str[0]))) disable_autosort = 0;
                        if ((!strcmp(ppath, setting_autosort_str)) && (!strcmp(args, onoff_str[1]))) disable_autosort = 1;

                        break;
                    case RML_MACROS:
                        if (macros_num < 256)
                        {
                            strncpy(macros[macros_num].name, ppath, 256);
                            strncpy(macros[macros_num].macro, args, 256);
                            makeSmall(macros[macros_num].name);
                            makeSmall(macros[macros_num].macro);
                            knownSignsOnly(macros[macros_num].name);
                            knownSignsOnly(macros[macros_num].macro);

                            macros_num++;
                        }
                        break;
                    case RML_NORMAL:
                        if (strchr(ppath, ':') != NULL) if(runPlugin(ppath, args, pmode) == 1) loaded++;
                        break;
                }
            }
            else if(runPrx(line) < 0x80000000) loaded++;
        }
       mud = 0;
        //log(line);
    }

    sceIoClose(fd);
    return loaded;
}

int checkPlugin(const char * attr)
{
    char * arg = (char *)attr;
    int foo = 1;
    int bar;
    for (; arg[0] == 0x20; arg++);
    bar = strlen(arg);
    for (foo = 1; arg[bar-foo] == 0x20; foo++) arg[bar-foo] = 0x0;
    foo = 1;
    if (arg[0] == 0x7E)
    {
        arg++;
        foo = -1;
    }
    if (!strncmp(arg, path_to_file, strlen(arg))) return foo;
    knownSignsOnly(arg);

    int modeOK = foo;

    if (arg[0] == 0x28)
    {
        arg++;
        if (memcmp(modes[mode], arg, 3) != 0) modeOK = 0;
        arg += 4;
    }

    if (!strcmp(modes[mode], arg)) return foo&modeOK;
    if (!strcmp(gametitle, arg)) return foo&modeOK;
    if (!strcmp(gameid, arg)) return foo&modeOK;

    int x;

    for(x = 0; x < macros_num; x++)
    {
        if(!strcmp(arg, macros[x].macro))
        {
            if (!strcmp(modes[mode], macros[x].name)) return foo&modeOK;
            if (!strcmp(gametitle, macros[x].name)) return foo&modeOK;
            if (!strcmp(gameid, macros[x].name)) return foo&modeOK;
        }
    }

    return 0;
}

int getMacros()
{


    int x = 0;
    int foo = 0;

    runModuleList("ms0:/seplugins/macros.txt", RML_MACROS);

    foo = macros_num;
#ifdef ENABLE_LOGGING
#ifdef ENABLE_LOGGING_ALL
    char text[64];
    sprintf (text, "%d macros read.", foo);
    logWrite(text);
#endif
#endif

    return 0;
}

int getGameInfo()
{
    char * isopath = 0;
    char * execFile = sceKernelInitFileName();
    char * foo;
    unsigned paramOffset;
    unsigned iconOffset;
    int fd;
    int size;
    int i;

    if (!execFile) return -1;

    memset(gameid,0x0,16);

    if (!strcmp("disc0:/PSP_GAME/SYSDIR/EBOOT.BIN", execFile) || !strcmp("disc0:/PSP_GAME/SYSDIR/BOOT.BIN", execFile))
    {

        i = sceUmdCheckMedium();
        if(i == 0)
        {
            sceUmdWaitDriveStat(PSP_UMD_PRESENT);
        }
        sceUmdActivate(1, "disc0:"); // Mount UMD to disc0: file system
        sceUmdWaitDriveStat(UMD_WAITFORINIT);
        fd = sceIoOpen("disc0:/PSP_GAME/PARAM.SFO", PSP_O_RDONLY, 0777);
        size = sceIoLseek(fd, 0, SEEK_END);
        sceIoLseek(fd, 0, SEEK_SET);
        if (size <= 0)
        {
            sceIoClose(fd);
            return -2;
        }

        isopath = (char *)sctrlSEGetUmdFile();

        if (!strncmp(isopath, "ms0:", 4))
        {
            mode = isoMode; strcpy(path_to_file, isopath);
            isodrv = getIsodrv();
        }
        else mode = umdMode;

    }

    else

    {
        fd = sceIoOpen(execFile, PSP_O_RDONLY, 0777);
        size = sceIoLseek(fd, 0, SEEK_END);
        sceIoLseek(fd, 0, SEEK_SET);
        if (size <= 0)
        {
            sceIoClose(fd);
            return -3;
        }
        if (((execFile[strlen(execFile)-1] == 'P') || (execFile[strlen(execFile)-1] == 'p'))
         && ((execFile[strlen(execFile)-2] == 'B') || (execFile[strlen(execFile)-2] == 'b'))
         && ((execFile[strlen(execFile)-3] == 'P') || (execFile[strlen(execFile)-3] == 'p'))
         && (execFile[strlen(execFile)-4] == '.')) mode = pbpMode;
        sceIoLseek(fd, 8, SEEK_SET);
        sceIoRead(fd, &paramOffset, 4);
        sceIoRead(fd, &iconOffset, 4);
        size = iconOffset - paramOffset;
        sceIoLseek(fd, paramOffset, SEEK_SET);

        strcpy(path_to_file, execFile);
    }
    SceUID memid = sceKernelAllocPartitionMemory(2, "paramsfo_buf", PSP_SMEM_Low, size+1, NULL);
    foo = sceKernelGetBlockHeadAddr(memid);
    memset(foo, 0, size+1);

    sceIoRead(fd, foo, size+1);
    sceIoClose(fd);

    psfGetKey("DISC_ID", foo, gameid);
    psfGetKey("TITLE", foo, gametitle);
    sceKernelFreePartitionMemory(memid);

    if (!strcmp(modes[mode], "iso") || !strcmp(modes[mode], "umd")) sceUmdDeactivate( 1, "disc0:" );

    makeSmall(path_to_file);

    return 0;
}

char * getLocalModsListName(char * path)
{
    char * pch;
    if (!strcmp(modes[mode], "iso"))
    {
        pch = path+(strlen(path)-4);
        sprintf(pch, "_plugins.txt");
    }

    if (!strcmp(modes[mode], "pbp"))
    {
        pch = path+(strlen(path)-9);
        sprintf(pch, "plugins.txt");
    }
    /*char temp [300];
    sprintf(temp, "%s\x0d\x0a", path);
    sceIoWrite(logd, temp, strlen(temp));*/
    return path;
}

int startup(SceSize args, void *argp)
{
#ifdef ENABLE_LOGGING
#ifdef ENABLE_LOGGING_ALL
    char text[256];
    logInit();
#endif
#endif

    runModuleList("ms0:/seplugins/sepulka.txt", RML_SETTIN);

    changeTimeStamp();

    getMacros();

/*#ifdef ENABLE_LOGGING
    if (!disable_filelog)
    {
        while (!sceKernelFindModuleByName ("sceKernelLibrary"))
        {
            sceKernelDelayThread (150000);
        }

        pspDebugScreenInitEx (0x44000000, 0, 0);
        unsigned int uiAddress = 0;
        unsigned int uiBufferWidth = 0;
        unsigned int uiPixelFormat = 0;
        unsigned int uiSync = 0;
        sceDisplayGetFrameBufferInternal (0, &uiAddress, &uiBufferWidth, &uiPixelFormat, &uiSync);
        sceDisplayGetMode (&uiPixelFormat, &uiBufferWidth, &uiBufferWidth);
        pspDebugScreenSetColorMode (uiPixelFormat);
        pspDebugScreenSetXY (0, 0);
        pspDebugScreenSetTextColor (0xFF0000FF);
    }
#endif*/

    if (mode == isoMode) runModuleList("ms0:/seplugins/sepulka.txt", RML_ISODRV);

    int foo = patchUmd();

#ifdef ENABLE_LOGGING
#ifdef ENABLE_LOGGING_ALL
    if (foo == 0) sprintf(text, "sceUmdDeactivate patched successfully!");
    else sprintf(text, "Couldn\'t patch sceUmdDeactivate. Error %i.", foo);
    logWrite(text);
    logWrite(" ");
#endif
#endif
    //Run main loadlist
    foo = runSepulkaTxt();
#ifdef ENABLE_LOGGING
#ifdef ENABLE_LOGGING_ALL
        if (foo < 0) sprintf(text, "Couldn\'t open main list. Error %i.", foo);
        if (foo == 1) sprintf(text, "1 plugin was loaded.");
        if (foo > 1 || foo == 0) sprintf(text, "%d plugins were loaded.", foo);
        logWrite(text);
        logWrite(" ");
#endif
#endif

    if (mode != unkMode || mode != umdMode)
    {
        foo = runModuleList(getLocalModsListName(path_to_file), 1);
#ifdef ENABLE_LOGGING
#ifdef ENABLE_LOGGING_ALL
        if (foo < 0) sprintf(text, "Couldn\'t open list.");
        if (foo == 1) sprintf(text, "1 plugin was loaded.");
        if (foo > 1) sprintf(text, "%d plugins were loaded.", foo);
        if (foo == 0) sprintf(text, "List is empty.");
        logWrite(text);
        logWrite(" ");
#endif
#endif
    }


    foo = unpatchUmd();

#ifdef ENABLE_LOGGING
#ifdef ENABLE_LOGGING_ALL
    if (foo == 0) sprintf(text, "sceUmdDeactivate unpatched successfully!");
    else sprintf(text, "Couldn\'t unpatch sceUmdDeactivate. Error %i.", foo);
    logWrite(" ");
    logWrite(text);

    //sceKernelDelayThread(2*1000*1000);

#endif
    logWrite(" ");
    logWrite("Exiting Sepulka...");
    logWrite("Have a nice play :)");
    if (log_inited) logInit();
#endif

//#ifdef ENABLE_LOGGING_ALL
    sceKernelDelayThread(10*1000*1000);
    runModuleList("ms0:/seplugins/sepulka.txt", RML_GAMPRO);
    //sprintf(text, "CPU: %d Bus: %d", scePowerGetCpuClockFrequency(), scePowerGetBusClockFrequency());

    int ret;

    sceKernelStopUnloadSelfModule(0, NULL, &ret, NULL);
    //sceKernelSelfStopUnloadModule(1,0,0);

    sceKernelExitDeleteThread(0);
    return 0;
}

int module_start(SceSize args, void *argp)
{
    getGameInfo();
    knownSignsOnly(gametitle);
    makeSmall(gametitle);
    knownSignsOnly(gameid);
    makeSmall(gameid);

    int thid = 0;

    thid = sceKernelCreateThread("Sepulka", startup, 0x10, 0x4000, 0, NULL);
    if(thid >= 0) {
            sceKernelStartThread(thid, 0, 0);
    }

	return 0;
}

int module_stop(void)
{
    //sceKernelUnloadModule(sceKernelGetModuleId());
    //sceKernelExitDeleteThread(0);
	return 0;
}
