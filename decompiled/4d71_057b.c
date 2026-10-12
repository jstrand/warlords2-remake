
void __cdecl16far random_map_link_nodes(void)

{
  int iVar1;
  char *pcVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  int local_8;
  char *local_6;
  int local_4;
  
  if (1 < *(int *)((int)DAT_451b_002a + 0x611e)) {
    iVar5 = 0;
    while( true ) {
      uVar6 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      if (*(int *)((int)DAT_451b_002a + 0x611e) <= iVar5) break;
      if (*(int *)((int)(undefined2 *)&side_upkeep + (int)DAT_451b_002a + iVar5 * 0xe) == 1) {
        iVar1 = dice(1,2,-1);
        if (*(int *)((int)DAT_451b_002a + 0x611e) + -1 <= iVar1) {
LAB_4d71_05f0:
          iVar1 = *(int *)((int)DAT_451b_002a + 0x611e) + -1;
        }
      }
      else {
        iVar1 = dice(1,4,-1);
        if (*(int *)((int)DAT_451b_002a + 0x611e) + -1 <= iVar1) goto LAB_4d71_05f0;
      }
      *(int *)((int)DAT_451b_002a + iVar5 * 0xe + 0x5ddc) = iVar1;
      for (local_4 = 0; local_4 < *(int *)((int)DAT_451b_002a + iVar5 * 0xe + 0x5ddc);
          local_4 = local_4 + 1) {
        local_6 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
        local_8 = -1;
        iVar1 = 0;
        while( true ) {
          uVar6 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
          iVar4 = (int)DAT_451b_002a;
          if (*(int *)(iVar4 + 0x611e) <= iVar1) break;
          if (iVar1 != iVar5) {
            iVar3 = 0;
            while ((iVar3 < local_4 && (*(int *)(iVar4 + iVar5 * 0xe + iVar3 * 2 + 0x5dde) != iVar1)
                   )) {
              iVar3 = iVar3 + 1;
            }
            if ((iVar3 == local_4) &&
               (pcVar2 = (char *)FUN_4c49_0fed(*(undefined4 *)(iVar4 + iVar1 * 0xe + 0x5dd6),
                                               *(undefined4 *)(iVar4 + iVar5 * 0xe + 0x5dd6)),
               (int)pcVar2 < (int)local_6)) {
              local_8 = iVar1;
              local_6 = pcVar2;
            }
          }
          iVar1 = iVar1 + 1;
        }
        *(int *)(iVar4 + iVar5 * 0xe + local_4 * 2 + 0x5dde) = local_8;
      }
      iVar5 = iVar5 + 1;
    }
  }
  return;
}


