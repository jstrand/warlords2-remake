
undefined2 __cdecl16far FUN_563e_134c(int param_1,int param_2)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_SS;
  byte local_18 [6];
  byte local_12 [6];
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  local_8 = 0;
  uVar4 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  local_a = *(int *)((int)DAT_451b_018e + param_1 * 0x5c + 0x250);
  local_c = *(int *)((int)DAT_451b_018e + param_1 * 0x5c + 0x24e);
  if (local_a != param_2) {
    ai_city_garrison_check(param_2,1);
    for (iVar2 = 0; iVar2 < 6; iVar2 = iVar2 + 1) {
      uVar4 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
      if (*(int *)((int)DAT_451b_018e + param_1 * 0x5c + iVar2 * 2 + 0x25a) == param_2) {
        *(undefined2 *)((int)DAT_451b_018e + param_1 * 0x5c + iVar2 * 2 + 0x25a) = 0xffff;
        break;
      }
    }
    local_4 = 0;
    FUN_5db9_0656(param_2,0,local_12);
    for (iVar2 = 0; iVar2 < 6; iVar2 = iVar2 + 1) {
      if ((local_12[iVar2] != 0xff) && (*(char *)((uint)local_12[iVar2] * 0x41 + 0x1592) == local_c)
         ) {
        if (local_18[iVar2] < 10) {
          local_4 = local_4 + 1;
        }
        if (local_18[iVar2] < 0x14) {
          local_4 = local_4 + 1;
        }
        if (local_18[iVar2] < 0x1e) {
          local_4 = local_4 + 1;
        }
        if (local_18[iVar2] < 0x32) {
          local_4 = local_4 + 1;
        }
      }
    }
    local_6 = 0;
    FUN_5db9_0656(local_a,0,local_12);
    for (iVar2 = 0; iVar2 < 6; iVar2 = iVar2 + 1) {
      if ((local_12[iVar2] != 0xff) && (*(char *)((uint)local_12[iVar2] * 0x41 + 0x1592) == local_c)
         ) {
        if (local_18[iVar2] < 10) {
          local_6 = local_6 + 1;
        }
        if (local_18[iVar2] < 0x14) {
          local_6 = local_6 + 1;
        }
        if (local_18[iVar2] < 0x1e) {
          local_6 = local_6 + 1;
        }
        if (local_18[iVar2] < 0x32) {
          local_6 = local_6 + 1;
        }
      }
    }
    if ((local_4 == 0) && (local_6 == 0)) {
      local_8 = 1;
    }
    iVar2 = local_a;
    if (local_6 < local_4) {
      iVar2 = param_2;
    }
    bVar1 = false;
    iVar3 = 0;
    while( true ) {
      uVar4 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
      if (5 < iVar3) break;
      if (*(int *)((int)DAT_451b_018e + param_1 * 0x5c + iVar3 * 2 + 0x292) == iVar2) {
        bVar1 = true;
        break;
      }
      iVar3 = iVar3 + 1;
    }
    if (!bVar1) {
      for (iVar3 = 0; iVar3 < 6; iVar3 = iVar3 + 1) {
        if (*(int *)((int)DAT_451b_018e + param_1 * 0x5c + iVar3 * 2 + 0x292) == -1) {
          *(int *)((int)DAT_451b_018e + param_1 * 0x5c + iVar3 * 2 + 0x292) = iVar2;
          break;
        }
      }
    }
    if (local_8 == 0) {
      uVar4 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
      *(int *)((int)DAT_451b_018e + param_1 * 0x5c + 0x250) = iVar2;
      *(undefined1 *)((int)DAT_451b_018e + param_2 + 0x56) = 7;
      FUN_563e_16fd(param_1);
      return 1;
    }
    FUN_563e_066d(param_1,3);
  }
  return 0;
}


