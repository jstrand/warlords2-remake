
void __cdecl16far FUN_17be_0001(int param_1,undefined2 param_2)

{
  bool bVar1;
  int iVar2;
  
  iVar2 = 0;
  bVar1 = false;
  while ((*(int *)((undefined1 *)&LAB_4125_155b_1 + iVar2 * 2) != 0 && (!bVar1))) {
    if (*(int *)((undefined1 *)&LAB_4125_155b_1 + iVar2 * 2) == param_1) {
      bVar1 = true;
      *(undefined2 *)((undefined1 *)&LAB_4125_155b_1 + iVar2 * 2) = param_2;
    }
    iVar2 = iVar2 + 1;
  }
  return;
}


