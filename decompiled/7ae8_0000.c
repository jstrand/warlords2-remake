
undefined2 __cdecl16far FUN_7ae8_0000(int param_1)

{
  int iVar1;
  undefined2 in_DX;
  int iVar2;
  int iVar3;
  
  iVar1 = FUN_7ae8_0198(0);
  iVar2 = *(int *)(iVar1 + 10);
  do {
    iVar3 = iVar2 + -1;
    if (iVar2 == 0) {
      return 0;
    }
    iVar2 = iVar3;
  } while (*(int *)((int)*(undefined4 *)(iVar1 + 4) + iVar3 * 8) != param_1);
  return *(undefined2 *)((int)*(undefined4 *)(iVar1 + 4) + iVar3 * 8 + 6);
}


