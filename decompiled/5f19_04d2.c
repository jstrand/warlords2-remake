
int __cdecl16far FUN_5f19_04d2(int param_1)

{
  int iVar1;
  int iVar2;
  byte *pbVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined2 unaff_SS;
  byte abStack_d [7];
  int local_6;
  int local_4;
  
  local_4 = 0;
  local_6 = -1;
  iVar4 = city_count;
LAB_5f19_0579:
  do {
    iVar5 = iVar4 + -1;
    if (iVar4 == 0) {
      return local_6;
    }
    iVar4 = iVar5;
  } while (*(char *)(iVar5 * 0x41 + 0x1592) != param_1);
  FUN_5db9_0656(iVar5,0,abStack_d + 1);
  iVar6 = 1;
  iVar1 = 6;
  while (iVar2 = iVar1 + -1, iVar1 != 0) {
    pbVar3 = abStack_d + iVar1;
    iVar1 = iVar2;
    if ((*pbVar3 != 0xff) && (*(char *)((uint)*pbVar3 * 0x41 + 0x1592) == param_1)) {
      iVar6 = iVar6 + 1;
    }
  }
  if (iVar6 <= local_4) goto code_r0x0005f6ea;
  goto LAB_5f19_0573;
code_r0x0005f6ea:
  if (iVar6 == local_4) {
    iVar1 = dice(1,2,-1);
    if (iVar1 != 0) {
LAB_5f19_0573:
      local_6 = iVar5;
      local_4 = iVar6;
    }
  }
  goto LAB_5f19_0579;
}


