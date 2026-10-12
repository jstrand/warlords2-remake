
uint __cdecl16far FUN_57ea_03aa(int param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  char cVar5;
  int iVar6;
  undefined2 uVar7;
  undefined2 unaff_SS;
  int local_2a [8];
  int local_1a [8];
  int local_a;
  uint local_8;
  int local_6;
  int local_4;
  
  iVar1 = FUN_57ea_0a3e(*(undefined2 *)(param_1 * 0x41 + 0x157d),
                        *(undefined2 *)(param_1 * 0x41 + 0x157f),0xffff);
  local_8 = (uint)(opt_hidden_map != 0);
  if (opt_neutral_cities == 0) {
    local_a = 1;
  }
  else {
    local_a = 8;
  }
  if (iVar1 < 5) {
    iVar2 = 2;
  }
  else {
    iVar2 = 0;
  }
  iVar2 = (uint)(iVar1 < 0xf) + iVar2;
  uVar7 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  if (*(int *)((int)DAT_451b_018e + 0x48) != 0) {
    iVar2 = iVar2 + 1;
  }
  iVar6 = 0;
  iVar1 = 8;
  while (iVar1 != 0) {
    iVar6 = iVar6 + *(int *)((int)DAT_451b_018e + (iVar1 + -1) * 2 + 0x40c);
    iVar1 = iVar1 + -1;
  }
  if (2 < iVar6) {
    iVar6 = 2;
  }
  local_4 = iVar2 + iVar6;
  iVar1 = 8;
  while (iVar3 = iVar1 + -1, iVar1 != 0) {
    local_2a[iVar3] = -1;
    local_1a[iVar3] = -1;
    iVar1 = iVar3;
  }
  iVar3 = 0;
  iVar1 = army_count;
  do {
    while( true ) {
      do {
        iVar4 = iVar1 + -1;
        if (iVar1 == 0) goto LAB_57ea_0565;
        iVar1 = iVar4;
      } while ((((*(char *)(iVar4 * 0x16 + -0x777b) != (char)current_player) ||
                ((*(byte *)(iVar4 * 0x16 + -0x7772) & 0x70) != 0)) ||
               ((*(uint *)(iVar4 * 0x16 + -0x7774) & 0x40) != 0)) ||
              ((((*(byte *)(iVar4 * 0x16 + -0x7772) & 0xf) != 0 ||
                (*(int *)(iVar4 * 0x16 + -0x7780) != *(int *)(param_1 * 0x41 + 0x157d))) ||
               (*(int *)(iVar4 * 0x16 + -0x777e) != *(int *)(param_1 * 0x41 + 0x157f)))));
      if (local_4 == 0) break;
      local_4 = local_4 + -1;
    }
    local_1a[iVar3] = iVar4;
    iVar3 = iVar3 + 1;
  } while (iVar3 < 8);
LAB_57ea_0565:
  if (iVar3 == 0) {
    local_8 = 0;
  }
  else {
    local_6 = 0;
    while( true ) {
      cVar5 = '\0';
      iVar1 = 8;
      iVar3 = -1;
      while (iVar4 = iVar1 + -1, iVar1 != 0) {
        iVar1 = iVar4;
        if ((local_1a[iVar4] != -1) && (cVar5 < *(char *)(local_1a[iVar4] * 0x16 + -0x777a))) {
          cVar5 = *(char *)(local_1a[iVar4] * 0x16 + -0x777a);
          iVar3 = iVar4;
        }
      }
      if (iVar3 == -1) break;
      local_2a[local_6] = local_1a[iVar3];
      local_6 = local_6 + 1;
      local_1a[iVar3] = -1;
    }
    while( true ) {
      iVar3 = 0;
      iVar1 = 8;
      while (iVar1 != 0) {
        local_1a[iVar1 + -1] = -1;
        iVar1 = iVar1 + -1;
      }
      iVar1 = 0;
      while ((iVar1 < 8 && (iVar3 < local_a))) {
        if (local_2a[iVar1] != -1) {
          local_1a[iVar3] = local_2a[iVar1];
          iVar3 = iVar3 + 1;
          local_2a[iVar1] = -1;
        }
        iVar1 = iVar1 + 1;
      }
      if ((iVar3 == 0) ||
         (((iVar2 + iVar6 == 0 && (opt_hidden_map != 0)) &&
          (iVar1 = FUN_57ea_0a3e(*(undefined2 *)(param_1 * 0x41 + 0x157d),
                                 *(undefined2 *)(param_1 * 0x41 + 0x157f),0xffff), iVar1 < 0xf))))
      break;
      FUN_57ea_06c7(param_1,iVar3,param_2,local_1a);
    }
  }
  return local_8;
}


