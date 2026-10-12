
void __cdecl16far FUN_6ef3_0947(undefined4 param_1)

{
  int iVar1;
  
  DAT_4125_0df6 = 4;
  FUN_6ef3_02fb();
  for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
    *(undefined2 *)((int)param_1 + iVar1 * 2) = ((undefined2 *)&DAT_451b_04f0)[iVar1];
  }
  return;
}


