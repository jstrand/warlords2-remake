
void __cdecl16far FUN_563e_1579(int param_1)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 unaff_SS;
  byte local_14 [6];
  int local_e;
  uint local_c;
  uint local_a;
  int local_8;
  undefined2 local_6;
  int local_4;
  
  local_8 = 0;
  local_a = 0xffff;
  iVar2 = 0;
  uVar5 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  iVar4 = (int)DAT_451b_018e;
  local_c = *(uint *)(iVar4 + param_1 * 0x5c + 0x250);
  local_e = *(int *)(iVar4 + param_1 * 0x5c + 0x24e);
  for (iVar3 = 0; iVar3 < 6; iVar3 = iVar3 + 1) {
    if (*(int *)(iVar4 + param_1 * 0x5c + iVar3 * 2 + 0x25a) != -1) {
      local_6 = *(undefined2 *)(iVar4 + param_1 * 0x5c + iVar3 * 2 + 0x25a);
      iVar2 = iVar2 + 1;
    }
  }
  if (iVar2 == 1) {
    FUN_5db9_0656(local_6,0,local_14);
    local_4 = 0;
    while( true ) {
      iVar2 = (int)DAT_451b_018e;
      uVar5 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
      if (5 < local_4) break;
      uVar1 = (uint)local_14[local_4];
      if ((uVar1 != 0xff) && ((*(byte *)(iVar2 + uVar1 + 0x11e) & 1) == 0)) {
        if (*(char *)(uVar1 * 0x41 + 0x1592) == local_e) {
          for (iVar3 = 0; iVar3 < 6; iVar3 = iVar3 + 1) {
            if (*(int *)(iVar2 + param_1 * 0x5c + iVar3 * 2 + 0x25a) == -1) {
              *(uint *)(iVar2 + param_1 * 0x5c + iVar3 * 2 + 0x25a) = uVar1;
              break;
            }
          }
        }
        else if (*(char *)(uVar1 * 0x41 + 0x1592) == current_player) {
          if (local_c == uVar1) {
            local_8 = 1;
          }
          iVar2 = FUN_5f19_07f8(uVar1);
          if ((iVar2 == 0) && (local_a == 0xffff)) {
            local_a = uVar1;
          }
        }
      }
      local_4 = local_4 + 1;
    }
    if ((local_8 == 0) && (local_a != 0xffff)) {
      *(uint *)(iVar2 + param_1 * 0x5c + 0x250) = local_a;
    }
  }
  return;
}


