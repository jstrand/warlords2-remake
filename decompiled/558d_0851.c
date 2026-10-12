
undefined2 __cdecl16far FUN_558d_0851(int param_1)

{
  byte bVar1;
  uint uVar2;
  byte bVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  int local_4;
  
  iVar5 = 0;
  iVar4 = (int)*(char *)(param_1 * 0x41 + 0x1592);
  if (iVar4 != 0xf) {
    iVar6 = 8;
    while (iVar7 = iVar6 + -1, iVar6 != 0) {
      iVar6 = iVar7;
      if ((*(int *)(iVar7 * 2 + 0x137) != 0) &&
         ((*(byte *)(current_player * 8 + iVar7 + 0x153b) & 3) == 2)) {
        iVar5 = iVar5 + 1;
        local_4 = iVar7;
      }
    }
    bVar1 = *(byte *)(current_player * 8 + iVar4 + 0x153b);
    bVar3 = bVar1 & 3;
    uVar8 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    if ((bVar1 & 3) == 0) {
      if (iVar5 != 0) {
        if (iVar5 != 1) {
          return 0;
        }
        if (local_4 != iVar4) {
          return 0;
        }
      }
      uVar2 = *(uint *)((int)DAT_451b_018e + 0xc);
    }
    else {
      if (bVar3 != 1) {
        if (bVar3 == 2) {
          return 1;
        }
        return 0;
      }
      uVar2 = *(uint *)((int)DAT_451b_018e + 0xc);
    }
    if ((uVar2 & 1) == 0) {
      return 0;
    }
  }
  return 1;
}


