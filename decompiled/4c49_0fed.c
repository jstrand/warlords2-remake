
int __cdecl16far FUN_4c49_0fed(int param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  
  uVar3 = param_1 - param_3 >> 0xf;
  iVar1 = (param_1 - param_3 ^ uVar3) - uVar3;
  uVar3 = param_2 - param_4 >> 0xf;
  iVar2 = (param_2 - param_4 ^ uVar3) - uVar3;
  if (iVar2 < iVar1) {
    iVar2 = iVar1;
  }
  return iVar2;
}


