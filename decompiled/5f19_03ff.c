
int __cdecl16far
FUN_5f19_03ff(undefined2 param_1,int param_2,undefined2 *param_3,undefined2 *param_4)

{
  char *pcVar1;
  int iVar2;
  int in_DX;
  int iVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  undefined2 uVar9;
  char *local_4;
  
  iVar2 = 0;
  for (iVar6 = 0; uVar9 = (undefined2)((ulong)DAT_451b_018e >> 0x10), iVar6 < 6; iVar6 = iVar6 + 1)
  {
    local_4 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
    iVar3 = city_count;
    iVar7 = -1;
    while (iVar4 = iVar3 + -1, iVar3 != 0) {
      iVar3 = iVar4;
      if ((*(char *)(iVar4 * 0x41 + 0x1592) == param_2) &&
         ((*(byte *)((int)DAT_451b_018e + iVar4 + 0x11e) & 1) == 0)) {
        uVar8 = (undefined2)((ulong)DAT_451b_01a2 >> 0x10);
        piVar5 = (int *)((int)DAT_451b_01a2 + iVar4 * 2);
        if ((*piVar5 != 0) && (pcVar1 = (char *)piVar5[100], (int)pcVar1 < (int)local_4)) {
          iVar7 = iVar4;
          local_4 = pcVar1;
        }
      }
    }
    in_DX = -1;
    if (iVar7 == -1) break;
    ((undefined2 *)param_3)[iVar6] = iVar7;
    ((undefined2 *)param_4)[iVar6] = local_4;
    *(undefined2 *)((int)DAT_451b_01a2 + iVar7 * 2) = 0;
    iVar2 = iVar2 + 1;
  }
  if ((iVar2 == 0) && ((*(byte *)((int)DAT_451b_018e + in_DX + 0x11e) & 1) == 0)) {
    *param_3 = param_1;
    *param_4 = 100;
    iVar2 = 1;
  }
  return iVar2;
}


