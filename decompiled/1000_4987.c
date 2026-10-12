
uint __cdecl16near __sgetc(int *param_1)

{
  int iVar1;
  byte *pbVar2;
  uint uVar3;
  
  iVar1 = ((int *)param_1)[1];
  pbVar2 = (byte *)*param_1;
  *param_1 = *param_1 + 1;
  if (*pbVar2 == 0) {
    uVar3 = 0xffff;
  }
  else {
    uVar3 = (uint)*pbVar2;
  }
  return uVar3;
}


