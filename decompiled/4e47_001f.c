
void __cdecl16far FUN_4e47_001f(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  
  iVar4 = 0;
  for (iVar2 = 0; iVar3 = (int)DAT_451b_002a, uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10),
      iVar2 < 0x70; iVar2 = iVar2 + 1) {
    for (iVar1 = 0; iVar1 < 0x9c; iVar1 = iVar1 + 1) {
      if (((*(char *)(iVar3 + iVar1 * 0x70 + iVar2 + 0x6120) == '\a') ||
          (*(char *)(iVar3 + iVar1 * 0x70 + iVar2 + 0x6120) == '\x05')) ||
         (*(char *)(iVar3 + iVar1 * 0x70 + iVar2 + 0x6120) == '\x06')) {
        iVar4 = iVar4 + 1;
      }
    }
  }
  *(int *)(iVar3 + 0x1f54) = (iVar4 / 100) * *(int *)(iVar3 + 0x3c);
  *(undefined2 *)(iVar3 + 0x1f52) = 0;
  return;
}


