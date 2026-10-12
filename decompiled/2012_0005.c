
void __cdecl16far FUN_2012_0005(int param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = 0;
  for (iVar1 = 0; iVar1 < param_2; iVar1 = iVar1 + param_3) {
    iVar3 = iVar4;
    for (iVar2 = 0; iVar2 < param_3; iVar2 = iVar2 + 1) {
      ((undefined2 *)&DAT_451b_2a78)[iVar1 + iVar2] = iVar3;
      iVar3 = iVar3 + param_4;
    }
    iVar4 = iVar4 + param_1 * 2;
  }
  return;
}


