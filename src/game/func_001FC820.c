#include "kf_player.h"
#include "kf_game.h"

extern PlayerState D_00413F80;
extern ClockTime D_00446A20;
extern ClockTime D_00446A28;
extern s32 D_00446DF4;
extern s32 D_00446DF8;
extern s32 D_00446DFC;
extern s32 D_00446E00;
extern s32 D_00446E04;
extern s32 D_00446E08;
extern s32 D_00446E0C;
extern s32 D_00446E10;
extern s32 D_00446E14;
extern s32 D_00446E18;
extern s32 D_00446E1C;
extern s32 D_00446E20;
extern s32 D_00446E24;
extern s32 D_00446E28;
extern s32 D_00446E2C;
extern s32 D_00446E30;
extern s32 D_00446E34;
extern s32 D_00446E38;
extern s32 D_00446E3C;
extern s32 D_00446E40;
extern s32 D_00446E44;
extern s32 D_00446E48;
extern s32 D_00446E4C;
extern s32 D_00446E50;
extern s32 D_00446E54;
extern s32 D_00446E58;
extern s32 D_00446E5C;
extern s32 D_00446E60;
extern s32 D_00446E64;
extern s32 D_00446E68;
extern s32 D_00446E6C;
extern s32 D_00446E70;
extern s32 D_00446E74;
extern s32 D_00446E78;
extern s32 D_00446E7C;
extern s32 D_00446E80;

/* Gives the current value of the number a menu sprite stands for (SpritePacket.unk34). */
int func_001FC820(int number) {
    switch (number) {
    case 0:
    case 64:
    case 65:
        return 0;
    case 1:
        return D_00413F80.unk58;
    case 2:
        return D_00413F80.unk5C;
    case 3:
        return D_00413F80.unk28;
    case 4:
        return D_00413F80.unk2C;
    case 5:
        return D_00413F80.unk60.values[0];
    case 6:
        return D_00413F80.unk60.values[1];
    case 7:
        return D_00413F80.unk60.values[2];
    case 8:
        return D_00413F80.unk60.values[3];
    case 9:
        return D_00413F80.unk60.values[4];
    case 10:
        return D_00413F80.unk60.values[5];
    case 11:
        return D_00413F80.unk60.values[6];
    case 12:
        return D_00413F80.unk60.values[7];
    case 13:
        return D_00413F80.unk60.values[8];
    case 14:
    case 15:
    case 16:
        return D_00413F80.unk90.values[0];
    case 17:
        return D_00413F80.unk90.values[1];
    case 18:
        return D_00413F80.unk90.values[2];
    case 19:
        return D_00413F80.unk90.values[3];
    case 20:
        return D_00413F80.unk90.values[4];
    case 21:
        return D_00413F80.unk90.values[5];
    case 22:
        return D_00413F80.unk90.values[6];
    case 23:
        return D_00413F80.unk90.values[7];
    case 24:
        return D_00413F80.unk90.values[8];
    case 25:
    case 26:
    case 27:
        return D_00413F80.unk4C;
    case 28:
        return D_00413F80.unk50;
    case 29:
        return D_00413F80.unk54;
    case 30:
        return D_00413F80.unkB4;
    case 31:
        return D_00413F80.unkB8;
    case 32:
        return D_00413F80.unkAC;
    case 33:
        return D_00413F80.unkB0;
    case 34:
        return D_00446A28.mday;
    case 35:
        return D_00446A28.hour;
    case 36:
        return D_00446A28.min;
    case 37:
        return D_00446A28.sec;
    case 38:
        return D_00446A20.year + 2000;
    case 39:
        return D_00446A20.mon;
    case 40:
        return D_00446A20.mday;
    case 41:
        return D_00446A20.hour;
    case 42:
        return D_00446A20.min;
    case 43:
        return D_00446A20.sec;
    case 44:
        return D_00446DF4;
    case 45:
        return D_00446DF8;
    case 46:
        return D_00446DFC;
    case 47:
        return D_00446E00;
    case 48:
        return D_00446E04;
    case 49:
        return D_00446E08;
    case 50:
        return D_00446E0C;
    case 51:
        return D_00446E10;
    case 52:
        return D_00446E14;
    case 53:
        return D_00446E18;
    case 54:
        return D_00446E1C;
    case 55:
        return D_00413F80.unk7E.values[0];
    case 56:
        return D_00413F80.unk7E.values[1];
    case 57:
        return D_00413F80.unk7E.values[2];
    case 58:
        return D_00413F80.unk7E.values[3];
    case 59:
        return D_00413F80.unk7E.values[4];
    case 60:
        return D_00413F80.unk7E.values[5];
    case 61:
        return D_00413F80.unk7E.values[6];
    case 62:
        return D_00413F80.unk7E.values[7];
    case 63:
        return D_00413F80.unk7E.values[8];
    case 66:
        return D_00413F80.unkA4;
    case 67:
        return D_00413F80.unkA8;
    case 70:
        return D_00446E20;
    case 71:
        return D_00446E24;
    case 72:
        return D_00446E28;
    case 73:
        return D_00446E2C;
    case 74:
        return D_00446E30;
    case 75:
        return D_00446E34;
    case 76:
        return D_00446E38;
    case 77:
        return D_00446E3C;
    case 78:
        return D_00446E40;
    case 79:
        return D_00446E44;
    case 80:
        return D_00446E48;
    case 81:
        return D_00446E4C;
    case 82:
        return D_00446E50;
    case 83:
        return D_00446E54;
    case 84:
        return D_00446E58;
    case 85:
        return D_00446E5C;
    case 86:
        return D_00446E60;
    case 87:
        return D_00446E64;
    case 88:
        return D_00446E68;
    case 89:
        return D_00446E6C;
    case 90:
        return D_00446E70;
    case 91:
        return D_00446E74;
    case 92:
        return D_00446E78;
    case 93:
        return D_00446E7C;
    case 94:
        return D_00446E80;
    default:
        return 0;
    }
}
