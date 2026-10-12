
int __cdecl16far FUN_18a9_03f5(int param_1)

{
  int iVar1;
  
  iVar1 = 0;
  while( true ) {
    if (*(int *)((undefined4 *)DAT_451b_0ce0)[2] <= iVar1) {
      return 0;
    }
    if (*(int *)((int)*DAT_451b_0ce0 + iVar1 * 0x21) == param_1) break;
    iVar1 = iVar1 + 1;
  }
  return iVar1;
}


