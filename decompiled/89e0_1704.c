
void __cdecl16far FUN_89e0_1704(void)

{
  int iVar1;
  
  for (iVar1 = 0; iVar1 < DAT_4125_30f6; iVar1 = iVar1 + 1) {
    ((undefined2 *)&DAT_451b_28f2)[iVar1] = iVar1;
    ((undefined2 *)&DAT_451b_28e2)[iVar1] = 0;
  }
  DAT_451b_28e2 = 1;
  FUN_89e0_17e7();
  FUN_89e0_0e85(0,0xffff0000);
  FUN_89e0_1425();
  return;
}


