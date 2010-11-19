#ifndef __SCTRLLIBRARY_SE_H__
#define __SCTRLLIBRARY_SE_H__

enum SEUmdModes
{
	MODE_UMD = 0,
	MODE_OE_LEGACY = 1,
	MODE_MARCH33 = 2,
	MODE_NP9660 = 3,
};

/*
typedef struct
{
	int magic; // 0x47434553 // 0
	int hidecorrupt; // 4
	int	skiplogo; // 8
	int umdactivatedplaincheck; // 12
	int gamekernel150; // 16
	int executebootbin; // 20
	int startupprog; // 24
	int umdmode; // 28
	int useisofsonumdinserted; // 32
	int	vshcpuspeed; // 36
	int	vshbusspeed; // 40
	int	umdisocpuspeed; // 44
	int	umdisobusspeed; // 48
	int fakeregion; // 52
	int freeumdregion; // 56
	int	hardresetHB;  // 60
	int usbdevice; // 64
	int novshmenu; // 68
	int usbcharge; // 72
	int notusedaxupd; // 76
	int hideiconpic; // 80
	int xmbplugins; // 84
	int gameplugins; // 88
	int popsplugins; // 92
	int useversion; // 96
	int speedupms; // 100; GEN -> hide mac addr
	int unk1; // 104
	int reserved[2]; // 108
} SEConfig;
*/

//int sctrlSEGetConfigEx(SEConfig *config, int size);

int sctrlSEMountUmdFromFile(char *file, int noumd, int isofs);

int sctrlSEUmountUmd(void);

void sctrlSESetDiscOut(int out);

char *sctrlSEGetUmdFile();

void sctrlSESetUmdFile(char *file);

void sctrlSESetBootConfFileIndex(int index);

#endif
