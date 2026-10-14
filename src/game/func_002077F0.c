#include "kf_player.h"
#include "kf_ini.h"

extern char D_004410A8[];
extern char D_00249680[];
extern IniKeyHandler D_0042B810[];

int sprintf(char *, char *, ...);
u32 strlen(char *);
int strncmp(const char *, const char *, u32);
char *strchr(const char *, int);
int func_00102238(char *, int, int);
int func_001025C0(int, void *, int);
int func_001024B0(int);

/* Reads the start-up settings ini/NNNN.ini (ini/9999.ini when that one is missing) and hands every line that
   starts with a key of D_0042B810 to the key's handler. */
void Ini_Load(int number) {
    char path[0x100];
    int file;
    int i;
    char *line;

    sprintf(path, D_004410A8, number);
    file = func_00102238((int)path, 0, 1);
    if (file < 0) {
        sprintf(path, D_004410A8, 9999);
        file = func_00102238((int)path, 0, 1);
        if (file < 0) {
            return;
        }
    }
    line = (char *)func_001025C0(file, D_00249680, -1);
    func_001024B0(file);
    Ini_SetText(D_00249680, (int)line);
    while ((line = Ini_ReadLine()) != NULL) {
        for (i = 0; D_0042B810[i].handler != NULL; i++) {
            if (strncmp(line, D_0042B810[i].key, strlen(D_0042B810[i].key)) == 0) {
                line = strchr(line, '=') + 1;
                D_0042B810[i].handler(line);
            }
        }
    }
}
