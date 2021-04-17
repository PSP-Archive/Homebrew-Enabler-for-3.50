#include <pspsdk.h>
#include <pspkernel.h>
#include <pspcrypt.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "decryptprx.h"


PSP_MODULE_INFO("NativeDecrypter", 0x1000, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

/* Code here taken from psppet's psardumper */


u8 updaters_keys[0x90] = 
{
	0xBF, 0x3C, 0x60, 0xA5, 0x41, 0x24, 0x48, 0xD7, 0xCC, 0x64, 0x57, 0xF6, 0x0B, 0x06, 0x90, 0x1F, 
	0x45, 0x3E, 0xA7, 0x4E, 0x92, 0xD1, 0x51, 0xE5, 0x8A, 0x5D, 0xB7, 0xE7, 0x6E, 0x50, 0x5A, 0x46, 
	0x22, 0x10, 0xFB, 0x40, 0x50, 0x33, 0x27, 0x2C, 0x44, 0xDA, 0x96, 0x80, 0x8E, 0x19, 0x47, 0x99, 
	0x77, 0xEE, 0x8D, 0x27, 0x2E, 0x06, 0x5D, 0x74, 0x45, 0xFA, 0x48, 0xC1, 0xAF, 0x82, 0x25, 0x83, 
	0xDA, 0x86, 0xDB, 0x5F, 0xCE, 0xC4, 0x15, 0xCB, 0x2F, 0xC6, 0x24, 0x25, 0xB1, 0xC3, 0x2E, 0x6C, 
	0x9E, 0xE3, 0x9B, 0x36, 0xC4, 0x1F, 0xEB, 0xF7, 0x1A, 0xCE, 0x51, 0x1E, 0xF4, 0x36, 0x05, 0xD7, 
	0xD8, 0x39, 0x4D, 0xC3, 0x13, 0xFB, 0x18, 0x74, 0xE1, 0x4D, 0xC8, 0xE3, 0x3C, 0xF0, 0x18, 0xB1, 
	0x4E, 0x8D, 0x01, 0xA2, 0x0D, 0x77, 0xD8, 0xE6, 0x90, 0xF3, 0x20, 0x57, 0x41, 0x63, 0xF9, 0x17, 
	0x8F, 0xA6, 0xA4, 0x60, 0x28, 0xDD, 0x27, 0x13, 0x64, 0x4C, 0x94, 0x05, 0x12, 0x4C, 0x2C, 0x0C
};

static unsigned long const g_key0[] =
{
  0x7b21f3be, 0x299c5e1d, 0x1c9c5e71, 0x96cb4645, 0x3c9b1be0, 0xeb85de3d, 
  0x4a7f2022, 0xc2206eaa, 0xd50b3265, 0x55770567, 0x3c080840, 0x981d55f2, 
  0x5fd8f6f3, 0xee8eb0c5, 0x944d8152, 0xf8278651, 0x2705bafa, 0x8420e533, 
  0x27154ae9, 0x4819aa32, 0x59a3aa40, 0x2cb3cf65, 0xf274466d, 0x3a655605, 
  0x21b0f88f, 0xc5b18d26, 0x64c19051, 0xd669c94e, 0xe87035f2, 0x9d3a5909, 
  0x6f4e7102, 0xdca946ce, 0x8416881b, 0xbab097a5, 0x249125c6, 0xb34c0872, 
};
static unsigned long const g_key2[] =
{
  0xccfda932, 0x51c06f76, 0x046dcccf, 0x49e1821e, 0x7d3b024c, 0x9dda5865, 
  0xcc8c9825, 0xd1e97db5, 0x6874d8cb, 0x3471c987, 0x72edb3fc, 0x81c8365d, 
  0xe161e33a, 0xfc92db59, 0x2009b1ec, 0xb1a94ce4, 0x2f03696b, 0x87e236d8, 
  0x3b2b8ce9, 0x0305e784, 0xf9710883, 0xb039db39, 0x893bea37, 0xe74d6805, 
  0x2a5c38bd, 0xb08dc813, 0x15b32375, 0x46be4525, 0x0103fd90, 0xa90e87a2, 
  0x52aba66a, 0x85bf7b80, 0x45e8ce63, 0x4dd716d3, 0xf5e30d2d, 0xaf3ae456, 
};
static unsigned long const g_key3[] =
{
  0xa6c8f5ca, 0x6d67c080, 0x924f4d3a, 0x047ca06a, 0x08640297, 0x4fd4a758, 
  0xbd685a87, 0x9b2701c2, 0x83b62a35, 0x726b533c, 0xe522fa0c, 0xc24b06b4, 
  0x459d1cac, 0xa8c5417b, 0x4fea62a2, 0x0615d742, 0x30628d09, 0xc44fab14, 
  0x69ff715e, 0xd2d8837d, 0xbeed0b8b, 0x1e6e57ae, 0x61e8c402, 0xbe367a06, 
  0x543f2b5e, 0xdb3ec058, 0xbe852075, 0x1e7e4dcc, 0x1564ea55, 0xec7825b4, 
  0xc0538cad, 0x70f72c7f, 0x49e8c3d0, 0xeda97ec5, 0xf492b0a4, 0xe05eb02a, 
};

typedef struct
{
    u32 tag; // 4 byte value at offset 0xD0 in the PRX file
    u8* key; // "step1_result" use for XOR step
    u8 code;
    u8 codeExtra;
} TAG_INFO;

static const TAG_INFO g_tagInfo[] =
{
    { 0x00000000, (u8*)g_key0, 0x42 },
    { 0x02000000, (u8*)g_key2, 0x45 },
    { 0x03000000, (u8*)g_key3, 0x46 },
    // updaters
    { 0x0b000000, updaters_keys, 0x4E },
};

static TAG_INFO const* GetTagInfo(u32 tagFind)
{
    int iTag;
    for (iTag = 0; iTag < sizeof(g_tagInfo)/sizeof(TAG_INFO); iTag++)
        if (g_tagInfo[iTag].tag == tagFind)
            return &g_tagInfo[iTag];
    return NULL; // not found
}

static void ExtraV2Mangle(u8* buffer1, u8 codeExtra)
{
    static u8 g_dataTmp[20+0xA0] __attribute__((aligned(0x40)));
    u8* buffer2 = g_dataTmp; // aligned

    memcpy(buffer2+20, buffer1, 0xA0);
    u32* pl2 = (u32*)buffer2;
    pl2[0] = 5;
    pl2[1] = pl2[2] = 0;
    pl2[3] = codeExtra;
    pl2[4] = 0xA0;

    int ret = semaphore_4C537C72(buffer2, 20+0xA0, buffer2, 20+0xA0, 7);
    if (ret != 0)
        ;//printf("extra de-mangle returns %d\n", ret);
    // copy result back
    memcpy(buffer1, buffer2, 0xA0);
}

int DecryptPRX(const u8* pbIn, u8* pbOut, int cbTotal, u32 tag)
{
    TAG_INFO const* pti = GetTagInfo(tag);
    if (pti == NULL)
        return -1;

    // build conversion into pbOut
    memcpy(pbOut, pbIn, cbTotal);
    memset(pbOut, 0, 0x150);
    memset(pbOut, 0x55, 0x40); // first $40 bytes ignored

    // step3 demangle in place
    u32* pl = (u32*)(pbOut+0x2C);
    pl[0] = 5; // number of ulongs in the header
    pl[1] = pl[2] = 0;
    pl[3] = pti->code; // initial seed for PRX
    pl[4] = 0x70;   // size

    // redo part of the SIG check (step2)
    u8 buffer1[0x150];
    memcpy(buffer1+0x00, pbIn+0xD0, 0x80);
    memcpy(buffer1+0x80, pbIn+0x80, 0x50);
    memcpy(buffer1+0xD0, pbIn+0x00, 0x80);
    if (pti->codeExtra != 0)
        ExtraV2Mangle(buffer1+0x10, pti->codeExtra);
    memcpy(pbOut+0x40 /* 0x2C+20 */, buffer1+0x40, 0x40);
    
    int ret;
    int iXOR;
    for (iXOR = 0; iXOR < 0x70; iXOR++)
        pbOut[0x40+iXOR] = pbOut[0x40+iXOR] ^ pti->key[0x14+iXOR];

    ret = semaphore_4C537C72(pbOut+0x2C, 20+0x70, pbOut+0x2C, 20+0x70, 7);
    if (ret != 0)
    {
        printf("mangle#7 returned $%x\n", ret);
        return -1;
    }

    for (iXOR = 0x6F; iXOR >= 0; iXOR--)
        pbOut[0x40+iXOR] = pbOut[0x2C+iXOR] ^ pti->key[0x20+iXOR];

    memset(pbOut+0x80, 0, 0x30); // $40 bytes kept, clean up
    pbOut[0xA0] = 1;
    // copy unscrambled parts from header
    memcpy(pbOut+0xB0, pbIn+0xB0, 0x20); // file size + lots of zeros
    memcpy(pbOut+0xD0, pbIn+0x00, 0x80); // ~PSP header

    // step4: do the actual decryption of code block
    //  point 0x40 bytes into the buffer to key info
    ret = semaphore_4C537C72(pbOut, cbTotal, pbOut+0x40, cbTotal-0x40, 0x1);
    if (ret != 0)
    {
        printf("mangle#1 returned $%x\n", ret);
        return -1;
    }

    // return cbTotal - 0x150; // rounded up size
	return *(u32*)&pbIn[0xB0]; // size of actual data (fix thanks to Vampire)
}

int decrypt_prx(u8 *inbuf, int insize, u8 *outbuf)
{
	int k1 = pspSdkSetK1(0);

	int size = DecryptPRX(inbuf, outbuf, insize, *(u32 *)&inbuf[0xD0]);
	
	pspSdkSetK1(k1);
	return size;
}


int module_start(SceSize args, void *argp)
{
	return 0;
}

int module_stop(void)
{
	return 0;
}