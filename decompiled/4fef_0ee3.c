
uint __cdecl16far FUN_4fef_0ee3(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  undefined2 uVar5;
  
  uVar4 = 0;
  for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
    uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    iVar3 = (int)DAT_451b_002a + iVar2 * 4;
    iVar1 = param_1 + *(int *)(iVar3 + 0xdc);
    iVar3 = param_2 + *(int *)(iVar3 + 0xde);
    if ((((-1 < iVar1) && (-1 < iVar3)) && (iVar1 < 0x70)) &&
       ((iVar3 < 0x9c &&
        (((*(byte *)(iVar3 * 0x70 + iVar1) & 0x1f) != 0 ||
         (*(char *)(*(byte *)(iVar3 * 0xe0 + iVar1 * 2) + 0x710) == '\x01')))))) {
      uVar4 = uVar4 | 1 << ((byte)iVar2 & 0x1f);
    }
  }
  return uVar4;
}


