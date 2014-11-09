#include <pspdebug.h>
#include <stdio.h>
#include <openssl/md5.h>
#include "doomdef.h"

#define printf pspDebugScreenPrintf

extern char		target[MAXPATH];

char calculated_md5_string[33];
char known_md5_string_hexen_beta_iwad[33] = "c88a2bb3d783e2ad7b599a8e301e099e";
char known_md5_string_hexen_demo_iwad[33] = "876a5a44c7b68f04b3bb9bc7a5bd69d6";
char known_md5_string_hexen_beta_demo_iwad[33] = "9178a32a496ff5befebfe6c47dac106c";
char known_md5_string_hexen_macdemo_iwad[33] = "925f9f5000e17dc84b0a6a3bed3a6f31";
char known_md5_string_hexen_macfull_iwad[33] = "b68140a796f6fd7f3a5d3226a32b93be";
char known_md5_string_hexen_1_0_iwad[33] = "b2543a03521365261d0a0f74d5dd90f0";
char known_md5_string_hexen_1_1_iwad[33] = "abb033caf81e26f12a2103e1fa25453f";
char known_md5_string_hexdd_1_0_iwad[33] = "1077432e2690d390c256ac908b5f4efa";
char known_md5_string_hexdd_1_1_iwad[33] = "78d5898e99e220e4de64edaa0e479593";

int MD5_Check()		// FOR PSP: THIS FUNCTION DEFINITELY WORKS, BUT IT WAS NEVER USED - MAYBE FUTURE
{
    int i;
    int bytes;
    char *filename = target;
    unsigned char c[MD5_DIGEST_LENGTH];
    unsigned char data[1024];

    FILE *inFile = fopen (filename, "rb");

    MD5_CTX mdContext;

//    if (inFile == NULL)
//    {
//        printf("%s can't be opened.\n", filename);

//        return 0;
//    }

    MD5_Init(&mdContext);

    while ((bytes = fread (data, 1, 1024, inFile)) != 0)
        MD5_Update (&mdContext, data, bytes);

    MD5_Final(c, &mdContext);

    for (i = 0; i < MD5_DIGEST_LENGTH; i++)
	sprintf(&calculated_md5_string[i * 2], "%02x", (unsigned int)c[i]);

//    if(strncmp(calculated_md5_string, known_md5_string, 32) == 0)
//	printf("\nMD5 MATCH!\n");
//    else
//	printf("\nMD5 FAIL!\n");

//    printf("%s\n", known_md5_string);

//    printf("%s\n", calculated_md5_string);

//    printf("%s\n", filename);

    fclose(inFile);

    return 0;
}

