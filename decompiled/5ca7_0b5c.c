
int __cdecl16far FUN_5ca7_0b5c(int param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  char cVar1;
  int iVar2;
  bool bVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  undefined2 uVar10;
  undefined2 uVar11;
  int local_18;
  int local_14;
  int local_12;
  int local_10;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  if (param_2 < 0xd) {
    local_4 = (int)*(char *)(param_2 + 0x824);
  }
  else {
    local_4 = 8;
  }
  local_10 = 0;
  uVar5 = 0;
  local_12 = 0;
  local_14 = 0;
  bVar3 = false;
  uVar10 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  iVar6 = (int)DAT_451b_018e;
  iVar7 = (int)param_3;
  uVar11 = (undefined2)((ulong)param_3 >> 0x10);
  if (*(char *)(iVar6 + param_1 + 0x56) == '\a') {
    iVar8 = 0;
    local_18 = 0;
    iVar9 = param_2;
    while (iVar4 = iVar9 + -1, iVar9 != 0) {
      iVar2 = *(int *)(iVar7 + iVar4 * 2);
      iVar9 = iVar4;
      if (iVar2 != -1) {
        if (*(char *)(iVar2 * 0x16 + -0x777c) == '\x1c') {
          local_18 = local_18 + 1;
        }
        else if ((((undefined1 *)&army_move_flags)[*(char *)(iVar2 * 0x16 + -0x777c) * 6] != '\0')
                && (('\x03' < *(char *)(iVar2 * 0x16 + -0x7778) || (iVar8 == 0)))) {
          iVar8 = iVar8 + 1;
        }
      }
    }
    if ((iVar8 != 0) && (local_18 != 0)) {
      iVar8 = iVar8 + local_18;
    }
    if (((3 < iVar8) && (param_2 < 0x11)) || ((2 < iVar8 && (param_2 < 9)))) {
      bVar3 = true;
    }
    iVar9 = 4;
    while (iVar8 = iVar9 + -1, iVar9 != 0) {
      iVar9 = iVar8;
      if ((*(int *)(iVar6 + iVar8 * 0x5c + 0x24c) != 0) &&
         (*(int *)(iVar6 + iVar8 * 0x5c + 0x250) == param_1)) {
        if ((*(uint *)(iVar6 + iVar8 * 0x5c + 0x2a6) & 0x10) == 0) {
          if ((*(uint *)(iVar6 + iVar8 * 0x5c + 0x2a6) & 8) == 0) {
            local_12 = *(int *)(iVar6 + iVar8 * 0x5c + 0x2a4);
          }
          else {
            local_12 = 0x10;
          }
        }
        else {
          local_12 = 0xc;
        }
        if ((*(uint *)(iVar6 + iVar8 * 0x5c + 0x2a6) & 0x20) != 0) {
          bVar3 = true;
        }
      }
    }
  }
  iVar6 = 3 - *(byte *)(iVar6 + param_1 + 0xba) / 3;
  if (((0 < iVar6) && (iVar6 < param_2)) && (param_2 - local_4 < iVar6)) {
    local_4 = param_2 - iVar6;
  }
  while( true ) {
    local_c = -1;
    local_a = -1;
    local_8 = -1;
    local_6 = -1;
    iVar6 = param_2;
    while (iVar9 = iVar6 + -1, iVar6 != 0) {
      iVar8 = *(int *)(iVar7 + iVar9 * 2);
      iVar6 = iVar9;
      if (iVar8 != -1) {
        iVar4 = (int)*(char *)(iVar8 * 0x16 + -0x7778);
        if (local_10 < local_4) {
          cVar1 = ((undefined1 *)&DAT_451b_0669)[*(char *)(iVar8 * 0x16 + -0x777c) * 6];
          if (((uVar5 & 1) == 0) && (*(char *)(iVar8 * 0x16 + -0x777c) == '\x1c')) {
            iVar4 = iVar4 + 1000;
          }
          else if (local_12 <= *(char *)(iVar8 * 0x16 + -0x777a)) {
            if (((uVar5 & 0x10) == 0) &&
               (((undefined1 *)&army_move_flags)[*(char *)(iVar8 * 0x16 + -0x777c) * 6] != '\0')) {
              iVar4 = iVar4 + 900;
            }
            else if (!bVar3) {
              if (((uVar5 & 2) == 0) && (cVar1 == '\x02')) {
                iVar4 = iVar4 + 800;
              }
              else if (((uVar5 & 4) == 0) && (cVar1 == '\x03')) {
                iVar4 = iVar4 + 700;
              }
              else if (((uVar5 & 0x80) == 0) &&
                      (((undefined1 *)&DAT_451b_0668)[*(char *)(iVar8 * 0x16 + -0x777c) * 6] != '\0'
                      )) {
                iVar4 = iVar4 + 600;
              }
              else if (((uVar5 & 8) == 0) && (cVar1 == '\x01')) {
                iVar4 = iVar4 + 500;
              }
              else if (((uVar5 & 0x20) == 0) &&
                      (((undefined1 *)&DAT_451b_0665)[*(char *)(iVar8 * 0x16 + -0x777c) * 6] != '\0'
                      )) {
                iVar4 = iVar4 + 400;
              }
              else if (((uVar5 & 0x40) == 0) &&
                      (((undefined1 *)&DAT_451b_0666)[*(char *)(iVar8 * 0x16 + -0x777c) * 6] != '\0'
                      )) {
                iVar4 = iVar4 + 300;
              }
            }
          }
        }
        else {
          uVar5 = 0xfff;
        }
        if (((uVar5 & 1) != 0) && (*(char *)(iVar8 * 0x16 + -0x777c) == '\x1c')) {
          iVar4 = 1;
        }
        if (((uVar5 & 0x80) != 0) &&
           (((undefined1 *)&DAT_451b_0668)[*(char *)(iVar8 * 0x16 + -0x777c) * 6] != '\0')) {
          iVar4 = 2;
        }
        if ((*(char *)(iVar8 * 0x16 + -0x777a) < local_12) ||
           (((bVar3 && (((undefined1 *)&army_move_flags)[*(char *)(iVar8 * 0x16 + -0x777c) * 6] ==
                        '\0')) && (*(char *)(iVar8 * 0x16 + -0x777c) != '\x1c')))) {
          if (local_8 < iVar4) {
            local_8 = iVar4;
            local_6 = iVar9;
          }
        }
        else if (local_c < iVar4) {
          local_c = iVar4;
          local_a = iVar9;
        }
      }
    }
    if ((local_6 == -1) && (local_a == -1)) break;
    if (uVar5 != 0xfff) {
      iVar6 = local_6;
      if (local_a != -1) {
        iVar6 = local_a;
      }
      iVar6 = *(int *)(iVar7 + iVar6 * 2);
      cVar1 = ((undefined1 *)&DAT_451b_0669)[*(char *)(iVar6 * 0x16 + -0x777c) * 6];
      if (*(char *)(iVar6 * 0x16 + -0x777c) == '\x1c') {
        uVar5 = uVar5 | 1;
      }
      else {
        if (cVar1 == '\x02') {
          uVar5 = uVar5 | 2;
        }
        if (cVar1 == '\x03') {
          uVar5 = uVar5 | 4;
        }
        if (((undefined1 *)&army_move_flags)[*(char *)(iVar6 * 0x16 + -0x777c) * 6] != '\0') {
          uVar5 = uVar5 | 0x10;
        }
        if (((undefined1 *)&DAT_451b_0668)[*(char *)(iVar6 * 0x16 + -0x777c) * 6] != '\0') {
          uVar5 = uVar5 | 0x80;
        }
        if (cVar1 == '\x01') {
          uVar5 = uVar5 | 8;
        }
        if (((undefined1 *)&DAT_451b_0665)[*(char *)(iVar6 * 0x16 + -0x777c) * 6] != '\0') {
          uVar5 = uVar5 | 0x20;
        }
        if (((undefined1 *)&DAT_451b_0666)[*(char *)(iVar6 * 0x16 + -0x777c) * 6] != '\0') {
          uVar5 = uVar5 | 0x40;
        }
      }
    }
    uVar10 = (undefined2)((ulong)param_4 >> 0x10);
    if (local_a == -1) {
      *(undefined2 *)((int)param_4 + local_10 * 2) = *(undefined2 *)(iVar7 + local_6 * 2);
      local_10 = local_10 + 1;
      *(undefined2 *)(iVar7 + local_6 * 2) = 0xffff;
    }
    else {
      *(undefined2 *)((int)param_4 + local_10 * 2) = *(undefined2 *)(iVar7 + local_a * 2);
      local_10 = local_10 + 1;
      *(undefined2 *)(iVar7 + local_a * 2) = 0xffff;
      local_14 = local_14 + 1;
    }
  }
  if (local_4 < local_14) {
    local_14 = local_4;
  }
  return local_14;
}


