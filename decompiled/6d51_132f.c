
int __cdecl16far FUN_6d51_132f(undefined4 param_1,int *param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_SS;
  int local_26 [16];
  int local_6;
  int local_4;
  
  iVar3 = 0;
  for (local_4 = 0; local_4 < 8; local_4 = local_4 + 1) {
    local_26[local_4 * 2 + 1] = 0;
    local_26[local_4 * 2] = 0;
  }
  for (local_4 = 0; local_4 < 8; local_4 = local_4 + 1) {
    if ('\0' < *(char *)(local_4 * 0x2b + 0x13e3)) {
      iVar3 = iVar3 + 1;
      local_26[local_4 * 2] = 1;
    }
  }
  for (local_4 = 0; local_4 < 8; local_4 = local_4 + 1) {
    if (('\x01' < *(char *)(local_4 * 0x2b + 0x13e3)) && (iVar3 < 10)) {
      iVar3 = iVar3 + 1;
      local_26[local_4 * 2 + 1] = 1;
    }
  }
  for (local_4 = 0; local_4 < 8; local_4 = local_4 + 1) {
    if ((local_26[local_4 * 2] != 0) && (local_26[local_4 * 2 + 1] == 0)) {
      *(undefined1 *)(local_4 * 0x2b + 0x13e3) = 1;
    }
  }
  local_6 = 0;
  for (local_4 = 0; local_4 < 8; local_4 = local_4 + 1) {
    for (iVar2 = 0; iVar2 < 2; iVar2 = iVar2 + 1) {
      if (local_26[iVar2 + local_4 * 2] != 0) {
        uVar4 = (undefined2)((ulong)param_1 >> 0x10);
        iVar1 = (int)param_1;
        *(undefined1 *)(iVar1 + local_6 * 0x16) = (undefined1)local_4;
        *(undefined1 *)(iVar1 + local_6 * 0x16 + 1) =
             *(undefined1 *)(local_4 * 0x2b + iVar2 + 0x13e4);
        *(undefined2 *)(iVar1 + local_6 * 0x16 + 2) =
             *(undefined2 *)(local_4 * 0x2b + iVar2 * 2 + 0x13e6);
        *(undefined2 *)(iVar1 + local_6 * 0x16 + 4) =
             *(undefined2 *)(local_4 * 0x2b + iVar2 * 2 + 0x13ea);
        strcpy((char *)CONCAT22(uVar4,(char *)(iVar1 + local_6 * 0x16 + 6)),
               (char *)CONCAT22(0x3c04,(char *)(local_4 * 0x2b + iVar2 * 0x10 + 0x13ee)));
        local_6 = local_6 + 1;
      }
    }
  }
  *param_2 = iVar3 * 0x16;
  return iVar3;
}


