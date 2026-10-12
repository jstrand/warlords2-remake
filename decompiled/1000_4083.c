
int __ftell_adjust(uint *param_1)

{
  char *pcVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  uint *puVar6;
  int iVar7;
  undefined2 uVar8;
  undefined4 local_6;
  
  uVar8 = (undefined2)((ulong)param_1 >> 0x10);
  puVar6 = (uint *)param_1;
  if ((int)*param_1 < 0) {
    iVar2 = puVar6[3] + *param_1 + 1;
  }
  else {
    uVar5 = (int)*param_1 >> 0xf;
    iVar2 = (*param_1 ^ uVar5) - uVar5;
  }
  iVar7 = iVar2;
  if ((puVar6[1] & 0x40) == 0) {
    local_6 = (char *)CONCAT22(puVar6[7],(char *)puVar6[6]);
    iVar3 = iVar2;
    if ((int)*param_1 < 0) {
      while (iVar3 = iVar2 + -1, iVar2 != 0) {
        local_6 = (char *)CONCAT22(local_6._2_2_,(char *)local_6 + -1);
        iVar2 = iVar3;
        if (*local_6 == '\n') {
          iVar7 = iVar7 + 1;
        }
      }
    }
    else {
      while (pcVar1 = local_6, iVar4 = iVar3 + -1, iVar7 = iVar2, iVar3 != 0) {
        local_6 = (char *)CONCAT22(local_6._2_2_,(char *)local_6 + 1);
        iVar3 = iVar4;
        if (*pcVar1 == '\n') {
          iVar2 = iVar2 + 1;
        }
      }
    }
  }
  return iVar7;
}


