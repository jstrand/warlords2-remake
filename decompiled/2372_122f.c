
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_2372_122f(undefined2 param_1,undefined2 param_2,undefined4 param_3)

{
  int iVar1;
  
  _DAT_451b_2fc4 = param_3;
  FUN_2372_0002(param_1,param_2);
  DAT_451b_300d = 0xff;
  DAT_451b_300b = 0xff;
  FUN_22bf_07a5();
  iVar1 = *(int *)((undefined4 *)DAT_451b_2fc8 + 2);
  while (iVar1 != 0) {
    *(undefined2 *)((int)*DAT_451b_2fc8 + (iVar1 + -1) * 0x2e + 0x1e) = 1;
    iVar1 = iVar1 + -1;
  }
  FUN_2372_1286();
  FUN_22bf_0757();
  return;
}


