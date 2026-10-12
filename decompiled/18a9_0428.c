
void __cdecl16far FUN_18a9_0428(int param_1,int param_2)

{
  int iVar1;
  
  iVar1 = 0;
  while( true ) {
    if (*(int *)((undefined4 *)DAT_451b_0ce0)[2] <= iVar1) {
      return;
    }
    if (*(int *)((int)*DAT_451b_0ce0 + iVar1 * 0x21) == param_1) break;
    iVar1 = iVar1 + 1;
  }
  if (*(int *)((int)*DAT_451b_0ce0 + iVar1 * 0x21 + 2) == param_2) {
    return;
  }
  *(int *)((int)*DAT_451b_0ce0 + iVar1 * 0x21 + 2) = param_2;
  *(undefined1 *)((int)*DAT_451b_0ce0 + iVar1 * 0x21 + 5) = 0;
  return;
}


