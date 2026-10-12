
int __cdecl16far
FUN_5f19_01bb(undefined2 param_1,undefined2 param_2,int *param_3,undefined2 *param_4)

{
  bool bVar1;
  int iVar2;
  undefined2 uVar3;
  char *pcVar4;
  int iVar5;
  int in_DX;
  int iVar6;
  int iVar7;
  int *piVar8;
  int iVar9;
  int iVar10;
  undefined2 uVar11;
  int local_6;
  char *local_4;
  
  local_6 = -1;
  iVar2 = 6;
  while( true ) {
    iVar5 = iVar2 + -1;
    uVar11 = (undefined2)((ulong)param_3 >> 0x10);
    if (iVar2 == 0) break;
    ((undefined2 *)param_4)[iVar5] = 0xffff;
    in_DX = iVar5 * 2;
    ((int *)param_3)[iVar5] = -1;
    iVar2 = iVar5;
  }
  iVar2 = mem_alloc(600,0,0,0);
  DAT_451b_01a6 = CONCAT22(in_DX,iVar2);
  if (iVar2 == 0 && in_DX == 0) {
    local_6 = -1;
  }
  else {
    uVar3 = mem_lock(DAT_451b_01a6);
    DAT_451b_01a2 = CONCAT22(in_DX,uVar3);
    iVar2 = FUN_5f19_04d2();
    if (iVar2 != -1) {
      FUN_5f19_058a(iVar2);
      local_4 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
      iVar5 = city_count;
      while( true ) {
        iVar10 = iVar5 + -1;
        uVar3 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
        if (iVar5 == 0) break;
        iVar5 = iVar10;
        if (*(char *)(iVar10 * 0x41 + 0x1592) == current_player) {
          bVar1 = false;
          iVar7 = 0;
          while (*(int *)(iVar7 * 2 + 0x940) != -1) {
            iVar6 = iVar7 + 1;
            iVar9 = iVar7 * 2;
            iVar7 = iVar6;
            if ((uint)*(byte *)((int)DAT_451b_018e + iVar10 + 0x56) == *(uint *)(iVar9 + 0x940)) {
              bVar1 = true;
            }
          }
          if (bVar1) {
            uVar3 = (undefined2)((ulong)DAT_451b_01a2 >> 0x10);
            piVar8 = (int *)((int)DAT_451b_01a2 + iVar10 * 2);
            if ((*piVar8 != 0) && (pcVar4 = (char *)piVar8[100], (int)pcVar4 < (int)local_4)) {
              local_6 = iVar10;
              local_4 = pcVar4;
            }
          }
        }
      }
      if (local_6 == -1) {
        if ((*(byte *)((int)DAT_451b_018e + iVar2 + 0x11e) & 1) == 0) {
          local_4 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
          iVar5 = city_count;
          while (iVar10 = iVar5 + -1, iVar5 != 0) {
            iVar5 = iVar10;
            if (*(char *)(iVar10 * 0x41 + 0x1592) == current_player) {
              bVar1 = false;
              iVar7 = 0;
              while (*(int *)(iVar7 * 2 + 0x94c) != -1) {
                iVar6 = iVar7 + 1;
                iVar9 = iVar7 * 2;
                iVar7 = iVar6;
                if ((uint)*(byte *)((int)DAT_451b_018e + iVar10 + 0x56) == *(uint *)(iVar9 + 0x94c))
                {
                  bVar1 = true;
                }
              }
              if ((bVar1) &&
                 (pcVar4 = (char *)map_distance(*(int *)(iVar10 * 0x41 + 0x157d),
                                                *(int *)(iVar10 * 0x41 + 0x157f),
                                                *(int *)(iVar2 * 0x41 + 0x157d),
                                                *(int *)(iVar2 * 0x41 + 0x157f)),
                 (int)pcVar4 < (int)local_4)) {
                local_6 = iVar10;
                local_4 = pcVar4;
              }
            }
          }
          if (local_6 != -1) {
            *param_3 = iVar2;
            *param_4 = 100;
          }
        }
      }
      else {
        FUN_5f19_058a(local_6,param_2);
        iVar2 = FUN_5f19_03ff(iVar2,param_2,(int *)param_3,uVar11,param_4);
        if (iVar2 == 0) {
          local_6 = -1;
        }
      }
    }
    FUN_1d28_048e(DAT_451b_01a6);
  }
  return local_6;
}


