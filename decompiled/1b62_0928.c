
void __cdecl16far FUN_1b62_0928(undefined4 param_1)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  
  iVar3 = 0;
  for (iVar2 = 0; uVar4 = (undefined2)((ulong)param_1 >> 0x10), iVar2 < 8; iVar2 = iVar2 + 1) {
    if (*(int *)((int)param_1 + iVar2 * 2) != -1) {
      iVar3 = iVar3 + 1;
    }
  }
  if (1 < iVar3) {
    iVar2 = 0;
    do {
      if (*(char *)(*(int *)((int)param_1 + iVar2 * 2) * 0x16 + -0x777c) <
          *(char *)(*(int *)((int)param_1 + (iVar2 + 1) * 2) * 0x16 + -0x777c)) {
        uVar1 = *(undefined2 *)((int)param_1 + iVar2 * 2);
        *(undefined2 *)((int)param_1 + iVar2 * 2) = *(undefined2 *)((int)param_1 + (iVar2 + 1) * 2);
        *(undefined2 *)((int)param_1 + (iVar2 + 1) * 2) = uVar1;
        iVar2 = 0;
      }
      else {
        iVar2 = iVar2 + 1;
      }
    } while (iVar2 < iVar3 + -1);
  }
  return;
}


