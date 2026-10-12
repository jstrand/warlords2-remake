
int __cdecl16far FUN_5311_03c3(undefined4 param_1)

{
  char *pcVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  undefined2 uVar9;
  char *local_8;
  int local_6;
  int local_4;
  
  local_4 = -1;
  local_6 = -1;
  uVar9 = (undefined2)((ulong)param_1 >> 0x10);
  iVar3 = (int)param_1;
  if (*(int *)(iVar3 + 400) == -1) {
    iVar7 = 0x32;
    while (iVar6 = iVar7 + -1, iVar7 != 0) {
      local_8 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
      iVar7 = iVar6;
      iVar2 = city_count;
      if (*(int *)(iVar3 + iVar6 * 2) != -1) {
        while (iVar8 = iVar2 + -1, iVar2 != 0) {
          piVar4 = (int *)(iVar3 + iVar6 * 2);
          pcVar1 = (char *)map_distance(*piVar4,piVar4[0x32],*(int *)(iVar8 * 0x41 + 0x157d),
                                        *(int *)(iVar8 * 0x41 + 0x157f));
          iVar2 = iVar8;
          if ((int)pcVar1 < (int)local_8) {
            local_8 = pcVar1;
          }
        }
        *(undefined2 *)(iVar3 + iVar6 * 2 + 400) = local_8;
      }
    }
  }
  iVar7 = 0x32;
  while (iVar6 = iVar7 + -1, iVar7 != 0) {
    local_8 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
    piVar4 = (int *)(iVar3 + iVar6 * 2);
    iVar7 = iVar6;
    if ((piVar4[0x96] == 0) && (*piVar4 != -1)) {
      iVar2 = 0x32;
      while (iVar8 = iVar2 + -1, iVar2 != 0) {
        iVar2 = iVar8;
        if ((*(int *)(iVar3 + iVar8 * 2 + 300) != 0) &&
           (piVar4 = (int *)(iVar3 + iVar8 * 2), piVar5 = (int *)(iVar3 + iVar6 * 2),
           pcVar1 = (char *)map_distance(*piVar5,piVar5[0x32],*piVar4,piVar4[0x32]),
           (int)pcVar1 < (int)local_8)) {
          local_8 = pcVar1;
        }
      }
      *(undefined2 *)(iVar3 + iVar6 * 2 + 500) = local_8;
    }
  }
  iVar7 = 0x32;
  while (iVar6 = iVar7 + -1, iVar7 != 0) {
    piVar4 = (int *)(iVar3 + iVar6 * 2);
    iVar7 = iVar6;
    if ((piVar4[0x96] == 0) && (*piVar4 != -1)) {
      iVar2 = dice(1,0xf,1);
      if (*(int *)(iVar3 + iVar6 * 2 + 400) < 0x1f) {
        iVar8 = 0x1e - *(int *)(iVar3 + iVar6 * 2 + 400);
      }
      else {
        iVar8 = 0;
      }
      if (local_6 < iVar8 + iVar2) {
        local_6 = iVar8 + iVar2;
        local_4 = iVar6;
      }
    }
  }
  return local_4;
}


