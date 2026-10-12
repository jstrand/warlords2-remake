
void __cdecl16far FUN_57ea_0b19(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  char cVar6;
  undefined2 uVar7;
  undefined2 unaff_SS;
  int local_2e [8];
  int local_1e [8];
  int local_e;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  local_6 = 0;
  local_a = *(int *)(param_1 * 0x41 + 0x157d);
  local_c = *(int *)(param_1 * 0x41 + 0x157f);
  iVar1 = FUN_57ea_0a3e(local_a,local_c,0xffff);
  local_e = 0;
  iVar2 = 4;
  while( true ) {
    iVar4 = iVar2 + -1;
    iVar3 = (int)DAT_451b_018e;
    uVar7 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    if (iVar2 == 0) break;
    iVar2 = iVar4;
    if (*(int *)(iVar3 + iVar4 * 0x5c + 0x24c) != 0) {
      local_e = 1;
    }
  }
  if (iVar1 < 5) {
    iVar2 = 2;
  }
  else {
    iVar2 = 0;
  }
  iVar2 = (uint)(iVar1 < 0xf) + iVar2;
  if (*(int *)(iVar3 + 0x48) != 0) {
    iVar2 = iVar2 + 1;
  }
  iVar4 = 0;
  iVar1 = 8;
  while (iVar1 != 0) {
    iVar4 = iVar4 + *(int *)(iVar3 + (iVar1 + -1) * 2 + 0x40c);
    iVar1 = iVar1 + -1;
  }
  if (2 < iVar4) {
    iVar4 = 2;
  }
  local_4 = iVar2 + iVar4;
  iVar1 = 8;
  while (iVar5 = iVar1 + -1, iVar3 = army_count, iVar1 != 0) {
    local_2e[iVar5] = -1;
    local_1e[iVar5] = -1;
    iVar1 = iVar5;
  }
  do {
    while( true ) {
      do {
        iVar1 = iVar3 + -1;
        if (iVar3 == 0) goto LAB_57ea_0d04;
        iVar3 = iVar1;
      } while ((((((*(char *)(iVar1 * 0x16 + -0x777b) != (char)current_player) ||
                  ((*(byte *)(iVar1 * 0x16 + -0x7772) & 0x70) != 0)) ||
                 ((*(uint *)(iVar1 * 0x16 + -0x7774) & 0x40) != 0)) ||
                (((*(char *)(iVar1 * 0x16 + -0x777c) == '\x1c' && (local_e != 0)) ||
                 (((*(byte *)(iVar1 * 0x16 + -0x7772) & 0xf) != 0 ||
                  ((param_2 != 0 &&
                   (((undefined1 *)&army_move_flags)[*(char *)(iVar1 * 0x16 + -0x777c) * 6] == '\0')
                   ))))))) || (*(int *)(iVar1 * 0x16 + -0x7780) != local_a)) ||
              (*(int *)(iVar1 * 0x16 + -0x777e) != local_c));
      if (local_4 == 0) break;
      local_4 = local_4 + -1;
    }
    local_1e[local_6] = iVar1;
    local_6 = local_6 + 1;
  } while (local_6 < 8);
LAB_57ea_0d04:
  if (local_6 == 0) {
    return;
  }
  local_8 = 0;
  while( true ) {
    cVar6 = '\0';
    iVar1 = 8;
    iVar3 = -1;
    while (iVar5 = iVar1 + -1, iVar1 != 0) {
      iVar1 = iVar5;
      if ((local_1e[iVar5] != -1) && (cVar6 < *(char *)(local_1e[iVar5] * 0x16 + -0x777a))) {
        cVar6 = *(char *)(local_1e[iVar5] * 0x16 + -0x777a);
        iVar3 = iVar5;
      }
    }
    if (iVar3 == -1) break;
    local_2e[local_8] = local_1e[iVar3];
    local_8 = local_8 + 1;
    local_1e[iVar3] = -1;
  }
  do {
    iVar1 = 8;
    while (iVar1 != 0) {
      local_1e[iVar1 + -1] = -1;
      iVar1 = iVar1 + -1;
    }
    for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
      if (local_2e[iVar1] != -1) {
        local_1e[0] = local_2e[iVar1];
        local_2e[iVar1] = -1;
        break;
      }
    }
    iVar1 = local_1e[0];
    if (local_1e[0] == -1) {
      return;
    }
    if (((iVar2 + iVar4 == 0) && (opt_hidden_map != 0)) &&
       (iVar3 = FUN_57ea_0a3e(*(undefined2 *)(param_1 * 0x41 + 0x157d),
                              *(undefined2 *)(param_1 * 0x41 + 0x157f),0xffff), iVar3 < 0xf)) {
      return;
    }
    FUN_57ea_0e4b(iVar1,param_1);
  } while( true );
}


