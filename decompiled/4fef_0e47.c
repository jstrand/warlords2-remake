
uint __cdecl16far FUN_4fef_0e47(int param_1,int param_2)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  uint local_4;
  
  local_4 = 0;
  for (iVar3 = 0; iVar3 < 8; iVar3 = iVar3 + 1) {
    uVar6 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    iVar4 = (int)DAT_451b_002a;
    iVar5 = iVar4 + iVar3 * 4;
    iVar2 = param_1 + *(int *)(iVar5 + 0xbc);
    iVar5 = param_2 + *(int *)(iVar5 + 0xbe);
    bVar1 = 0x9b < iVar5 || (0x6f < iVar2 || (iVar5 < 0 || iVar2 < 0));
    if ((*(char *)(iVar4 + iVar5 * 0x70 + iVar2 + 0x6120) == '\x06') ||
       (*(char *)(iVar4 + iVar5 * 0x70 + iVar2 + 0x6120) == '\x05')) {
      bVar1 = true;
    }
    if (bVar1) {
      local_4 = local_4 | 1 << ((byte)iVar3 & 0x1f);
    }
  }
  return local_4;
}


