#include <stdio.h>
#include <stdlib.h>         //built with codeblocks and mingw
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>                       //probably don't need most of these :p
#include <stdint.h>
#include <string.h>
#include <Windows.h>

void clear_screen(char fill = ' ') {
    COORD tl = {0,0};
    CONSOLE_SCREEN_BUFFER_INFO s;
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(console, &s);
    DWORD written, cells = s.dwSize.X * s.dwSize.Y;
    FillConsoleOutputCharacter(console, fill, cells, tl, &written);
    FillConsoleOutputAttribute(console, s.wAttributes, cells, tl, &written);
    SetConsoleCursorPosition(console, tl);
}

bool is3DSVersion = false;

char msgsecName[16];
char msgsecPath[16];
uint16_t msgSize = 0;

uint16_t datHeader[0x400] = {0};
char msgBuffer[0x100];

off_t getFileSize(const char *fileName)
{
    FILE* fp = fopen(fileName, "rb");
    off_t fsize = 0;
    if (fp) {
        fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);			// Get source file's size
		fseek(fp, 0, SEEK_SET);
	}
	fclose(fp);

	return fsize;
}

void makeDatFiles(bool oneFile) {
    int iStart = 0;
    int iEnd = 184;

    clear_screen();
    if (oneFile) {
        int typedNumber[5];
        int typedNumberPos = 0;
        bool increaseTypedNumberPos = false;

        printf("Type the 3-digit number:\n");
        while (1) {
            if (GetKeyState('0') & 0x8000) {
                printf("0");
                typedNumber[typedNumberPos] = 0;
                increaseTypedNumberPos = true;
                while (GetKeyState('0') & 0x8000);
            }
            if (GetKeyState('1') & 0x8000) {
                printf("1");
                typedNumber[typedNumberPos] = 1;
                increaseTypedNumberPos = true;
                while (GetKeyState('1') & 0x8000);
            }
            if (GetKeyState('2') & 0x8000) {
                printf("2");
                typedNumber[typedNumberPos] = 2;
                increaseTypedNumberPos = true;
                while (GetKeyState('2') & 0x8000);
            }
            if (GetKeyState('3') & 0x8000) {
                printf("3");
                typedNumber[typedNumberPos] = 3;
                increaseTypedNumberPos = true;
                while (GetKeyState('3') & 0x8000);
            }
            if (GetKeyState('4') & 0x8000) {
                printf("4");
                typedNumber[typedNumberPos] = 4;
                increaseTypedNumberPos = true;
                while (GetKeyState('4') & 0x8000);
            }
            if (GetKeyState('5') & 0x8000) {
                printf("5");
                typedNumber[typedNumberPos] = 5;
                increaseTypedNumberPos = true;
                while (GetKeyState('5') & 0x8000);
            }
            if (GetKeyState('6') & 0x8000) {
                printf("6");
                typedNumber[typedNumberPos] = 6;
                increaseTypedNumberPos = true;
                while (GetKeyState('6') & 0x8000);
            }
            if (GetKeyState('7') & 0x8000) {
                printf("7");
                typedNumber[typedNumberPos] = 7;
                increaseTypedNumberPos = true;
                while (GetKeyState('7') & 0x8000);
            }
            if (GetKeyState('8') & 0x8000) {
                printf("8");
                typedNumber[typedNumberPos] = 8;
                increaseTypedNumberPos = true;
                while (GetKeyState('8') & 0x8000);
            }
            if (GetKeyState('9') & 0x8000) {
                printf("9");
                typedNumber[typedNumberPos] = 9;
                increaseTypedNumberPos = true;
                while (GetKeyState('9') & 0x8000);
            }
            if (increaseTypedNumberPos) {
                typedNumberPos++;
                if (typedNumberPos == 3) {
                    printf("\n\nPress any number to continue.\n");
                }
                if (typedNumberPos > 3) {
                    iStart = ((typedNumber[0]*100)+(typedNumber[1]*10)+(typedNumber[2]));
                    iEnd = iStart;
                    break;
                }
                increaseTypedNumberPos = false;
            }
        }
        clear_screen();
    }
	printf("Making...\n");

    chdir(is3DSVersion ? "3DS" : "DS");
    FILE* fp;
    FILE* newDat;
    // Process all .dat files
    for (int i = iStart; i <= iEnd; i++) {
        if (i == 62) i = 100;
        if (i >= 100) {
            sprintf(msgsecName, "msgsec%i", i);
        } else if (i >= 10) {
            sprintf(msgsecName, "msgsec0%i", i);
        } else {
            sprintf(msgsecName, "msgsec00%i", i);
        }
        sprintf(msgsecPath, "%s.dat", msgsecName);
        newDat = fopen(msgsecPath, "wb");
        chdir(msgsecName);
        // Clear header
        for (int j = 0; j < 0x400; j++) {
            datHeader[j] = 0;
        }
        // Construct header
        for (int j = 0; j < 1024; j++) {
            sprintf(msgsecPath, "%i.msg", j+1);
            if (access(msgsecPath, F_OK) == 0) {
                // msg found
                if (j > 0) {
                    sprintf(msgsecPath, "%i.msg", j);
                    datHeader[j+1] = (datHeader[j] + getFileSize(msgsecPath));
                }
                datHeader[0]++;
            } else {
                break;
            }
        }
        // Fix message addresses
        for (int j = 0; j < datHeader[0]; j++) {
            datHeader[j+1] += (datHeader[0]+1)*2;
        }
        // Write header
        fwrite(datHeader, sizeof(uint16_t), datHeader[0]+1, newDat);
        // Write .msg files into .dat file
        for (int j = 0; j < datHeader[0]; j++) {
            sprintf(msgsecPath, "%i.msg", j+1);
            fp = fopen(msgsecPath, "rb");
            fread(msgBuffer, 1, 0x100, fp);
            fclose(fp);
            fwrite(msgBuffer, 1, getFileSize(msgsecPath), newDat);
        }
        chdir("..");
        fclose(newDat);
    }
}

void extractDatFiles(bool oneFile) {
    int iStart = 0;
    int iEnd = 184;

    clear_screen();
    if (oneFile) {
        int typedNumber[5];
        int typedNumberPos = 0;
        bool increaseTypedNumberPos = false;

        printf("Type the 3-digit number:\n");
        while (1) {
            if (GetKeyState('0') & 0x8000) {
                printf("0");
                typedNumber[typedNumberPos] = 0;
                increaseTypedNumberPos = true;
                while (GetKeyState('0') & 0x8000);
            }
            if (GetKeyState('1') & 0x8000) {
                printf("1");
                typedNumber[typedNumberPos] = 1;
                increaseTypedNumberPos = true;
                while (GetKeyState('1') & 0x8000);
            }
            if (GetKeyState('2') & 0x8000) {
                printf("2");
                typedNumber[typedNumberPos] = 2;
                increaseTypedNumberPos = true;
                while (GetKeyState('2') & 0x8000);
            }
            if (GetKeyState('3') & 0x8000) {
                printf("3");
                typedNumber[typedNumberPos] = 3;
                increaseTypedNumberPos = true;
                while (GetKeyState('3') & 0x8000);
            }
            if (GetKeyState('4') & 0x8000) {
                printf("4");
                typedNumber[typedNumberPos] = 4;
                increaseTypedNumberPos = true;
                while (GetKeyState('4') & 0x8000);
            }
            if (GetKeyState('5') & 0x8000) {
                printf("5");
                typedNumber[typedNumberPos] = 5;
                increaseTypedNumberPos = true;
                while (GetKeyState('5') & 0x8000);
            }
            if (GetKeyState('6') & 0x8000) {
                printf("6");
                typedNumber[typedNumberPos] = 6;
                increaseTypedNumberPos = true;
                while (GetKeyState('6') & 0x8000);
            }
            if (GetKeyState('7') & 0x8000) {
                printf("7");
                typedNumber[typedNumberPos] = 7;
                increaseTypedNumberPos = true;
                while (GetKeyState('7') & 0x8000);
            }
            if (GetKeyState('8') & 0x8000) {
                printf("8");
                typedNumber[typedNumberPos] = 8;
                increaseTypedNumberPos = true;
                while (GetKeyState('8') & 0x8000);
            }
            if (GetKeyState('9') & 0x8000) {
                printf("9");
                typedNumber[typedNumberPos] = 9;
                increaseTypedNumberPos = true;
                while (GetKeyState('9') & 0x8000);
            }
            if (increaseTypedNumberPos) {
                typedNumberPos++;
                if (typedNumberPos == 3) {
                    printf("\n\nPress any number to continue.\n");
                }
                if (typedNumberPos > 3) {
                    iStart = ((typedNumber[0]*100)+(typedNumber[1]*10)+(typedNumber[2]));
                    iEnd = iStart;
                    break;
                }
                increaseTypedNumberPos = false;
            }
        }
        clear_screen();
    }
	printf("Extracting...\n");

    chdir(is3DSVersion ? "3DS" : "DS");
    FILE* fp;
    FILE* extractedMsg;
    // Process all .dat files
    for (int i = iStart; i <= iEnd; i++) {
        if (i == 62) i = 100;
        if (i >= 100) {
            sprintf(msgsecName, "msgsec%i", i);
        } else if (i >= 10) {
            sprintf(msgsecName, "msgsec0%i", i);
        } else {
            sprintf(msgsecName, "msgsec00%i", i);
        }
        mkdir(msgsecName);
        sprintf(msgsecPath, "%s.dat", msgsecName);
        fp = fopen(msgsecPath, "rb");
        chdir(msgsecName);
        fread(datHeader, sizeof(uint16_t), 0x400, fp);
        // Extract messages from .dat file
        for (int j = 0; j < datHeader[0]; j++) {
            sprintf(msgsecPath, "%i.msg", j+1);
            fseek(fp, datHeader[j+1], SEEK_SET);
            fread(msgBuffer, 1, 0x100, fp);
            msgSize = 0;
            for (int k = 0; k < 0x100; k++) {
                msgSize++;
                if ((msgBuffer[k] == 0x05) && (msgBuffer[k+1] == 0x05) && (msgBuffer[k+2] == 0x05)) {
                    // End of message
                    msgSize += 2;
                    break;
                }
            }
            extractedMsg = fopen(msgsecPath, "wb");
            fwrite(msgBuffer, 1, msgSize, extractedMsg);
            fclose(extractedMsg);
        }
        chdir("..");
        fclose(fp);
    }
    chdir("..");
}

#define titleText "FabStyle Msg Tool v1.0\nby RocketRobz\n"

void displayTitle(void) {
    clear_screen();
	printf(titleText);
	printf("\n");
	printf(is3DSVersion ? "Set to read .dat files in 3DS folder\n" : "Set to read .dat files in DS folder\n");
	printf("\n");
	printf(is3DSVersion ? "C: Switch to DS\n" : "C: Switch to 3DS\n");
	printf("M: Make .dat files, K: Make one .dat file\n");
	printf("X: Extract from .dat files, D: Extract one .dat file\n");
}

int main(int argc, char **argv) {

	displayTitle();

    while (1) {
        if (GetKeyState('C') & 0x8000) {
            is3DSVersion = !is3DSVersion;
            displayTitle();
        }
        if (GetKeyState('M') & 0x8000) {
            makeDatFiles(false);
            break;
        }
        if (GetKeyState('K') & 0x8000) {
            makeDatFiles(true);
            break;
        }
        if (GetKeyState('X') & 0x8000) {
            extractDatFiles(false);
            break;
        }
        if (GetKeyState('D') & 0x8000) {
            extractDatFiles(true);
            break;
        }
    }

    clear_screen();
	printf("Done!\n");

	return 0;
}

