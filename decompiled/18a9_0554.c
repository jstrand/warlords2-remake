
void __cdecl16far FUN_18a9_0554(int param_1,undefined1 param_2)

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
  *(undefined1 *)((int)*DAT_451b_0ce0 + iVar1 * 0x21 + 4) = param_2;
  *(undefined1 *)((int)*DAT_451b_0ce0 + iVar1 * 0x21 + 5) = 1;
  return;
}


