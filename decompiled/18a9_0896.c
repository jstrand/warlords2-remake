
/* WARNING: Type propagation algorithm not settling */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __cdecl16far FUN_18a9_0896(void)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int x1;
  int y1;
  int iVar7;
  undefined2 uVar8;
  int local_6;
  int local_4;
  
  FUN_22bf_0af4(&local_6);
  if (DAT_4125_3700 != 0) {
    return 0;
  }
  if (_LAB_4125_1176 != 0) {
    return 4;
  }
  if (_LAB_4125_1172 != 0) {
    return 0xb;
  }
  if (DAT_4125_166a != 0) {
    return 0;
  }
  if (DAT_4125_2a86 == 0) {
    return 0;
  }
  if (*(int *)(current_player * 2 + 0xd0) != 0) {
    return 0;
  }
  iVar2 = FUN_216d_00b8(0x2aa8,0x4125,&local_6);
  if (iVar2 != 0) {
    if ((bRam00000417 & 8) != 0) {
      return 0xb;
    }
    return 1;
  }
  iVar2 = FUN_216d_00b8((undefined4 *)&DAT_4125_2ab0,0x4125,&local_6);
  if (iVar2 == 0) {
    iVar2 = FUN_216d_00b8(0x2ab8,0x4125,&local_6);
    if (iVar2 == 0) {
      return 0;
    }
  }
  else {
    x1 = DAT_3c04_0177 + (local_6 + -0x10) / 0x28;
    y1 = DAT_3c04_0175 + (local_4 + -0x1e) / 0x28;
    uVar3 = *(byte *)(y1 * 0xe0 + x1 * 2 + 1) & 0xf;
    iVar2 = (int)*(char *)(*(byte *)(y1 * 0xe0 + x1 * 2) + 0x710);
    if (DAT_451b_1f02 == (int *)0x0) {
      iVar4 = 0;
    }
    else {
      iVar4 = map_distance(x1,y1,*DAT_451b_1f02,((int *)DAT_451b_1f02)[1]);
    }
    if (DAT_451b_1f02 == (int *)0x0) {
      cVar1 = '\a';
    }
    else {
      cVar1 = *(char *)(*(byte *)(((int *)DAT_451b_1f02)[1] * 0xe0 + *DAT_451b_1f02 * 2) + 0x710);
    }
    uVar5 = (*(byte *)(y1 * 0xe0 + x1 * 2 + 1) & 0x10) >> 4;
    iVar6 = FUN_8611_15ff(1,x1,y1);
    if ((DAT_451b_1f02 == (int *)0x0) || (DAT_451b_1e9c == 2)) {
      iVar7 = 1;
    }
    else {
      iVar7 = *(int *)((undefined1 *)&terrain_move_cost + iVar2 * 2);
    }
    if ((*(byte *)(y1 * 0x70 + x1) & 0x1f) != 0) {
      iVar7 = 1;
    }
    if ((iVar6 != 0) && (0 < iVar7)) {
      if ((bRam00000417 & 3) == 0) {
        if (((uVar5 != 0) && (current_player == uVar3)) && ((bRam00000417 & 4) == 0)) {
          if ((bRam00000417 & 8) != 0) {
            return 0xb;
          }
          if ((DAT_451b_1f02 == (int *)0x0) || (iVar4 < 1)) {
            return 5;
          }
joined_r0x0001971a:
          if ((DAT_451b_1e9c != 2) && ((iVar2 == 2 || (iVar2 == 3)))) {
            return 2;
          }
          return 6;
        }
        if (((DAT_451b_1f02 != (int *)0x0) && (iVar4 < 2)) &&
           ((current_player != uVar3 && ((iVar2 == 10 || (uVar5 != 0)))))) {
          uVar8 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
          if ((((((int *)DAT_451b_1f02)[6] & 0x1000U) == 0) ||
              (((((DAT_451b_1e9c != 1 || (cVar1 != '\x03')) || (iVar2 == 3)) ||
                ((iVar2 == 2 || (iVar2 == 10)))) || (iVar2 == 1)))) &&
             ((((((int *)DAT_451b_1f02)[6] & 0x1000U) != 0 || (DAT_451b_1e9c != 1)) ||
              ((iVar2 != 3 ||
               ((((cVar1 == '\x03' || (cVar1 == '\x02')) || (cVar1 == '\n')) || (cVar1 == '\x01'))))
              )))) {
            if ((((opt_diplomacy != 0) && (uVar3 != 0xf)) &&
                ((iVar2 != 10 || ((*(byte *)(current_player * 8 + uVar3 + 0x153b) & 3) != 2)))) &&
               (((iVar2 == 10 && ((*(byte *)(current_player * 8 + uVar3 + 0x153b) & 3) != 2)) ||
                ((*(byte *)(current_player * 8 + uVar3 + 0x153b) & 3) == 0)))) {
              return 10;
            }
            return 8;
          }
        }
        if ((DAT_451b_1f02 != (int *)0x0) &&
           ((current_player == uVar3 || ((uVar3 == 0xf && (iVar2 != 10)))))) {
          if ((bRam00000417 & 8) != 0) {
            return 0xb;
          }
          if (((0 < iVar4) && ((bRam00000417 & 4) != 0)) && (uVar5 != 0)) {
            return 5;
          }
          goto joined_r0x0001971a;
        }
      }
      else if (((DAT_451b_1f02 != (int *)0x0) && (iVar4 < 2)) &&
              ((current_player != uVar3 &&
               (((iVar2 == 10 || (uVar5 != 0)) && (opt_military_advisor != 0)))))) {
        return 9;
      }
      if (iVar2 == 10) {
        return 3;
      }
      if (iVar2 == 0xb) {
        return 7;
      }
    }
  }
  return 4;
}


