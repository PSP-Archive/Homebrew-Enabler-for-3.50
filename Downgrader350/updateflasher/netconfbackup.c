/*
 * main.c - Building on the sample in pspsdk for netconf by John Kelley
 *          adding in writting of network configs for 1.00-3.0x
 *
 * http://www.noobz.eu/
 */
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <stdio.h>
#include <string.h>
#include <pspmoduleinfo.h>
#include <psputility.h>
#include <pspctrl.h>
#include <psputility_netparam.h>
#include <stdlib.h>

// 2.x settings
#define PSP_NETPARAM_8021X_TYPE			18 
#define PSP_NETPARAM_8021X_AUTH_NAME	19
#define PSP_NETPARAM_8021X_AUTH_KEY		20
#define PSP_NETPARAM_WPA_KEY_TYPE		21
#define PSP_NETPARAM_WPA_KEY			22
#define PSP_NETPARAM_BROWSER_FLAG		23

// 3.0x setings
#define PSP_NETPARAM_WIFISVC_CONFIG		24
#define PSP_NETPARAM_WIFISVC_AUTH_NAME	25
#define PSP_NETPARAM_WIFISVC_AUTH_KEY	26
#define PSP_NETPARAM_WIFISVC_OPTION		27

//Just to leave room for expansion
#define NUM_NETPARAMS 28

// Take into account the different counts for firmware versions
#define FIRMWARE_1XX_NETCONF 16
#define FIRMWARE_2XX_NETCONF 24
#define FIRMWARE_30X_NETCONF 28

#define CONNECTION	NUM_NETPARAMS

// Get the number of entries on a per-firmware basis
int GetNetparamCount(void);

void HexToAscii( char *source, char *dest, unsigned long size );

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
    sceKernelExitGame();

	return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
    int cbid;
    cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();

	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
    int thid = 0;
    thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
    if (thid >= 0)
	sceKernelStartThread(thid, 0, 0);
    return thid;
}

const char *ConnectionSettingNames[] = {
	"name",					// 0
	"ssid",					// 1
	"secure",				// 2
	"wepkey",				// 3
	"staticip",				// 4
	"ip",					// 5
	"netmask",				// 6
	"router",				// 7
	"manualdns",			// 8
	"primarydns",			// 9
	"secondarydns",			// 10
	"pppoe_user",			// 11
	"pppoe_pass",			// 12
	"useproxy",				// 13
	"proxyserver",			// 14
	"proxyport",			// 15
	"unknown",				// 16
	"unknown",				// 17
	"8021x_type",			// 18
	"8021x_auth_name",		// 19
	"8021x_auth_key",		// 20
	"wpa_key_type",			// 21
	"wpa_key",				// 22
	"browser_flag",			// 23
	"wifisvc_config",		// 24
	"wifisvc_auth_name",	// 25
	"wifisvc_auth_key",		// 26
	"wifisvc_option"		// 27
};

struct ConnectionSettings
{
	int ConnectionNum;
	netData data[NUM_NETPARAMS];
};

// Fill in a default structure to reset the entries
struct ConnectionSettings dflt_settings = {
	0,					// int ConnectionNum,
	{
		{asString: ""},					// char ConnectionName[128],
		{asString: ""},					// char SSID[128],
		{asUint: 0},					// int Secure
		{asString: ""},					// char WepKey[128];
		{asUint: 0},					// int StaticIp
		{asString: "0.0.0.0"},			// char IPAddr[128];
		{asString: "255.255.255.0"},	// char Netmask[128];
		{asString: "0.0.0.0"},			// char Router[128];
		{asUint: 0},					// int ManualDNS,
		{asString: "0.0.0.0"},			// char PrimDNS[128];
		{asString: "0.0.0.0"},			// char SecDNS[128];
		{asString: ""},					// char ProxyUser[128];
		{asString: ""},					// char ProxyPass[128];
		{asUint: 0},					// int UseProxy
		{asString: ""},					// char ProxyServer[128];
		{asUint: 8080},					// int ProxyPort;
		{asString: ""},					// not written
		{asString: ""},					// not written
		{asUint: 0},					// 8021x_type
		{asString: ""},					// 8021x_auth_name
		{asString: ""},					// 8021x_auth_key
		{asUint: 0},					// wpa_key_type
		{asString: ""},					// wpa_key
		{asUint: 0},					// browser_flag
		{asUint: 0},					// wifisvc_config
		{asString: ""},					// wifisvc_auth_name
		{asString: ""},					// wifisvc_auth_key
		{asUint: 0}						// wifisvc_option
	}
};

char *KeyToString(char *key, int count)
{
	static char temp[512];
	int i;
	
	temp[0] = 0;

	for(i=0; i<count; i++)
	{
		sprintf(&temp[i*3], "%02x ", (unsigned char)key[i]);
	}

	return temp;
}

void HexToAscii( char *source, char *dest, unsigned long size )
{
	char currentByte = 0x00, newByte = 0x00, tempByte = 0x00;
	unsigned long sourceOffset = 0, destOffset = 0;
	while( sourceOffset < size )
	{
		currentByte = *(source + sourceOffset);
		if( currentByte >= 0x30 && currentByte <= 0x39 )		newByte = currentByte - 0x30;	// 0 to 9
		else if( currentByte >= 0x41 && currentByte <= 0x46 )	newByte = currentByte - 0x37;	// A to F
		else if( currentByte >= 0x61 && currentByte <= 0x66 )	newByte = currentByte - 0x57;	// a to f
		else 													newByte = 0x00;
		newByte <<= 4;
		currentByte = *(source + sourceOffset +1);
		if( currentByte >= 0x30 && currentByte <= 0x39 )		tempByte = currentByte - 0x30;	// 0 to 9
		else if( currentByte >= 0x41 && currentByte <= 0x46 )	tempByte = currentByte - 0x37;	// A to F
		else if( currentByte >= 0x61 && currentByte <= 0x66 )	tempByte = currentByte - 0x57;	// a to f
		else 													tempByte = 0x00;
		newByte += tempByte & 0x0F;
		*(dest + destOffset++) = newByte;
		sourceOffset += 3;
	}
}

void BackupNetconf(char *file)
{
	char config[4096];
	char temp[2048];
	int i, netConfIndex;
	netData data;
	int secureValue=0;

	config[0] = 0;
	temp[0] = 0;

    for (netConfIndex=1; netConfIndex<100; netConfIndex++)
	{
		if(sceUtilityCheckNetParam(netConfIndex) == 0)
		{
			sprintf(temp, "connection=%d\n", netConfIndex);
			strcat(config, temp);

	    	for (i=0; i<GetNetparamCount(); i++)
			{
    			//clear netData
    			data.asUint = 0xBADF00D;
    			memset(&data.asString[4], 0, 124);
	    		if(sceUtilityGetNetParam(netConfIndex, i, &data) == 0)
				{
					switch(i)
					{
						case PSP_NETPARAM_NAME:
						case PSP_NETPARAM_SSID:
						case PSP_NETPARAM_NETMASK:
						case PSP_NETPARAM_ROUTE:
						case PSP_NETPARAM_PROXY_USER:
						case PSP_NETPARAM_PROXY_PASS:
						case PSP_NETPARAM_SECONDARYDNS:
						case PSP_NETPARAM_IP:
						case PSP_NETPARAM_PRIMARYDNS:
						case PSP_NETPARAM_PROXY_SERVER:
						case PSP_NETPARAM_8021X_AUTH_NAME:
						case PSP_NETPARAM_8021X_AUTH_KEY:
						case PSP_NETPARAM_WIFISVC_AUTH_NAME:
						case PSP_NETPARAM_WIFISVC_AUTH_KEY:
						{
							if(strcmp(data.asString, dflt_settings.data[i].asString) != 0)
							{
								sprintf(temp, "%s=%s\n", ConnectionSettingNames[i],
										data.asString);
								strcat(config, temp);
							}
							break;
						}

						case PSP_NETPARAM_IS_STATIC_IP:
						case PSP_NETPARAM_MANUAL_DNS:
						case PSP_NETPARAM_USE_PROXY:
						case PSP_NETPARAM_PROXY_PORT:
						case PSP_NETPARAM_8021X_TYPE:
						case PSP_NETPARAM_BROWSER_FLAG:
						case PSP_NETPARAM_WIFISVC_CONFIG:
						case PSP_NETPARAM_WIFISVC_OPTION:
						case PSP_NETPARAM_WPA_KEY_TYPE:
						{
							if(data.asUint != dflt_settings.data[i].asUint)
							{
								sprintf(temp, "%s=%d\n", ConnectionSettingNames[i],
											data.asUint);
								strcat(config, temp);
							}
							break;
						}

						case PSP_NETPARAM_SECURE:
						{
							if(strcmp(data.asString, dflt_settings.data[i].asString) != 0)
							{
								sprintf(temp, "%s=%d\n", ConnectionSettingNames[i],
											data.asUint);
								strcat(config, temp);
								secureValue = data.asUint;
							}
							break;
						}

						case PSP_NETPARAM_WEPKEY:
						{
							if(strcmp(data.asString, dflt_settings.data[i].asString) != 0)
							{
								int count=0;
								if(secureValue==1)
								{
									count = 5;
								}
								else if(secureValue==2)
								{
									count = 13;
								}

								sprintf(temp, "%s=%s\n", ConnectionSettingNames[i],
											KeyToString(data.asString, count));
								strcat(config, temp);
							}
							break;
						}

						case PSP_NETPARAM_WPA_KEY:
						{
							if(strcmp(data.asString, dflt_settings.data[i].asString) != 0)
							{
								sprintf(temp, "%s=%s\n", ConnectionSettingNames[i],
											KeyToString(data.asString, 64));
								strcat(config, temp);
							}
							break;
						}
					}
				}
			}
    	}
	}

	int fd = sceIoOpen(file, PSP_O_WRONLY|PSP_O_CREAT, 0777);
	if(fd)
	{
		sceIoWrite(fd, config, strlen(config));

		sceIoClose(fd);
	}
}

int GetNetparamCount(void)
{
	switch(sceKernelDevkitVersion())
	{
		case 0x01000300: case 0x01050001: case 0x01050100: case 0x01050200:
		{
			return FIRMWARE_1XX_NETCONF;
		}

		case 0x02000010: case 0x02050010: case 0x02060010: case 0x02070010:
		case 0x02070110: case 0x02080010: case 0x02080110: case 0x02080210:
		{
			return FIRMWARE_2XX_NETCONF;
		}
		
		case 0x03000010: case 0x03000110: case 0x03000210:
		{
			return FIRMWARE_30X_NETCONF;
		}
	}
	return NUM_NETPARAMS;
}
