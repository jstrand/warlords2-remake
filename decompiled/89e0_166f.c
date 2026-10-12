
void __cdecl16far FUN_89e0_166f(int param_1)

{
  int iVar1;
  int iVar2;
  
  iVar1 = ((undefined2 *)&DAT_451b_28f2)[param_1];
  for (iVar2 = 0; iVar2 < DAT_4125_30f6; iVar2 = iVar2 + 1) {
    ((undefined2 *)&DAT_451b_28e2)[iVar2] = (uint)(((undefined2 *)&DAT_451b_28f2)[iVar2] == iVar1);
  }
  FUN_89e0_17e7();
  FUN_89e0_0e85(0,0xffff0000);
  FUN_89e0_1425();
  return;
}


