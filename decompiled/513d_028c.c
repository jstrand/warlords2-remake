
void __cdecl16far FUN_513d_028c(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  
  for (iVar1 = 0; iVar1 < 0x70; iVar1 = iVar1 + 1) {
    for (iVar3 = 0; iVar3 < 0x9c; iVar3 = iVar3 + 1) {
      uVar4 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      iVar2 = (int)DAT_451b_002a;
      if (((*(char *)(iVar2 + iVar3 * 0x70 + iVar1 + 0x6120) == '\n') &&
          (*(char *)(iVar2 + (iVar3 + -1) * 0x70 + iVar1 + 0x6120) != '\n')) &&
         (*(char *)(iVar2 + iVar3 * 0x70 + iVar1 + 0x611f) != '\n')) {
        *(int *)(iVar2 + 0xa84) = *(int *)(iVar2 + 0xa84) + 1;
        *(int *)(city_count * 0x41 + 0x157d) = iVar1;
        *(int *)(city_count * 0x41 + 0x157f) = iVar3;
        city_count = city_count + 1;
      }
    }
  }
  return;
}


