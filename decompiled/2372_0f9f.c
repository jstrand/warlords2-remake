
undefined2 __cdecl16far FUN_2372_0f9f(int param_1)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  byte local_8;
  
  uVar4 = (undefined2)((ulong)DAT_451b_2fc8 >> 0x10);
  piVar1 = (int *)DAT_451b_2fc8;
  iVar2 = piVar1[5];
  do {
    iVar3 = iVar2 + -1;
    if (iVar2 == 0) {
      return 0;
    }
    iVar2 = iVar3;
  } while ((*(int *)((int)*(undefined4 *)(piVar1 + 2) + iVar3 * 8) != param_1) ||
          (local_8 = (byte)*(undefined2 *)((int)*(undefined4 *)(piVar1 + 2) + iVar3 * 8 + 4),
          (*(ulong *)(*DAT_451b_2fc8 +
                      *(int *)((int)*(undefined4 *)(piVar1 + 2) + iVar3 * 8 + 2) * 0x2e + 0x1a) &
          (long)(1 << (local_8 & 0x1f))) == 0));
  return 1;
}


