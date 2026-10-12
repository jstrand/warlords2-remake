
int __cdecl16far FUN_17be_0037(int param_1)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  iVar1 = 0;
  while ((*(int *)((undefined1 *)&LAB_4125_155b_1 + iVar2 * 2) != 0 && (iVar1 == 0))) {
    if (*(int *)((undefined1 *)&LAB_4125_155b_1 + iVar2 * 2) == param_1) {
      iVar1 = 1;
    }
    iVar2 = iVar2 + 1;
  }
  return iVar1;
}


