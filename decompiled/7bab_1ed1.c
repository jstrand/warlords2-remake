
void __cdecl16far FUN_7bab_1ed1(int param_1)

{
  int iVar1;
  
  for (iVar1 = 0; iVar1 < 5; iVar1 = iVar1 + 1) {
    ((undefined2 *)&DAT_451b_19fa)[iVar1] = ((undefined2 *)&DAT_451b_19fa)[iVar1] + param_1;
    if ((int)((undefined2 *)&DAT_451b_19fa)[iVar1] < 0) {
      ((undefined2 *)&DAT_451b_19fa)[iVar1] = 0xffff;
    }
    if (DAT_451b_08ce <= (int)((undefined2 *)&DAT_451b_19fa)[iVar1]) {
      ((undefined2 *)&DAT_451b_19fa)[iVar1] = 0xffff;
    }
  }
  FUN_7bab_1be6(1);
  FUN_7bab_1ae4();
  return;
}


