
undefined2 __cdecl16far FUN_4c49_117d(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  
  iVar1 = 0;
  while( true ) {
    if (7 < iVar1) {
      return 0;
    }
    uVar3 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    iVar2 = (int)DAT_451b_002a + iVar1 * 4;
    if ((((SCARRY2(param_1,*(int *)(iVar2 + 0xbc)) == param_1 + *(int *)(iVar2 + 0xbc) < 0) &&
         (SCARRY2(param_2,*(int *)(iVar2 + 0xbe)) == param_2 + *(int *)(iVar2 + 0xbe) < 0)) &&
        (param_1 + *(int *)(iVar2 + 0xbc) < 0x70)) &&
       ((param_2 + *(int *)(iVar2 + 0xbe) < 0x9c &&
        (*(char *)((int)DAT_451b_002a + (param_2 + *(int *)(iVar2 + 0xbe)) * 0x70 + param_1 +
                   *(int *)(iVar2 + 0xbc) + 0x6120) == '\a')))) break;
    iVar1 = iVar1 + 1;
  }
  return 1;
}


