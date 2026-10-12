
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_6087_0427(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  bool bVar4;
  int local_4;
  
  bVar4 = *(char *)(*(byte *)(param_2 * 0xe0 + param_1 * 2) + 0x710) != '\n';
  if (_DAT_451b_1efe != 0) {
    iVar2 = 0x16;
    while (iVar3 = iVar2 + -1, iVar2 != 0) {
      iVar2 = iVar3;
      if (((*(char *)(iVar3 * 0x1d + 0xcff) == '\x01') &&
          ((!bVar4 ||
           ((*(int *)(iVar3 * 0x1d + 0xd02) == param_1 &&
            (*(int *)(iVar3 * 0x1d + 0xd04) == param_2)))))) &&
         ((bVar4 || ((((param_1 <= *(int *)(iVar3 * 0x1d + 0xd02) &&
                       (*(int *)(iVar3 * 0x1d + 0xd02) <= param_1 + 1)) &&
                      (param_2 <= *(int *)(iVar3 * 0x1d + 0xd04))) &&
                     (*(int *)(iVar3 * 0x1d + 0xd04) <= param_2 + 1)))))) {
        *(undefined1 *)(iVar3 * 0x1d + 0xcff) = 3;
        local_4 = army_count;
        do {
          iVar1 = local_4 + -1;
          if (local_4 == 0) break;
          local_4 = iVar1;
        } while ((_DAT_451b_1f00 != 0x2e1d) || (iVar1 * 0x16 + -0x7780 != DAT_451b_1efe));
        *(int *)(iVar3 * 0x1d + 0xd00) = iVar1;
      }
    }
  }
  return;
}


