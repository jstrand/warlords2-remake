
void __cdecl16far FUN_2372_0eab(int param_1,undefined2 param_2)

{
  int iVar1;
  
  if (param_1 == 0) {
    iVar1 = *(int *)((undefined4 *)DAT_451b_2fc8 + 2);
    while (iVar1 != 0) {
      FUN_2372_0e17(*(undefined2 *)((int)*DAT_451b_2fc8 + (iVar1 + -1) * 0x2e + 8),0,param_2);
      iVar1 = iVar1 + -1;
    }
  }
  else {
    FUN_2372_0e17(param_1,0,param_2);
  }
  return;
}


