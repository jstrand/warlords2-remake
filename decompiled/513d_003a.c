
void __cdecl16far auto_str_03d_is_inhabited_by(void)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 unaff_SS;
  char *pcVar6;
  undefined4 uVar7;
  char local_112 [256];
  char local_12 [10];
  int local_8;
  int local_6;
  int local_4;
  
  iVar4 = 0;
  site_count = 0x28;
  for (iVar3 = 0; iVar3 < 0x28; iVar3 = iVar3 + 1) {
    *(undefined2 *)(iVar3 * 0x1f + 0x813) = 0xffff;
    *(undefined2 *)(iVar3 * 0x1f + 0x811) = 0xffff;
  }
  for (iVar3 = 0; iVar3 < site_count; iVar3 = iVar3 + 1) {
    iVar1 = (iVar3 * 10) / site_count;
    if (iVar4 != iVar1) {
      random_map_progress();
      iVar4 = iVar1;
    }
    FUN_513d_0a77(7,&local_4);
    *(int *)(iVar3 * 0x1f + 0x811) = local_4;
    *(int *)(iVar3 * 0x1f + 0x813) = local_6;
    *(undefined1 *)((int)DAT_451b_002a + local_6 * 0x70 + local_4 + 0x6120) = 0xb;
    if (*(char *)(iVar3 * 0x1f + 0x829) == '\x01') {
      iVar1 = dice(1,10,-1);
      sprintf((char *)CONCAT22(unaff_SS,local_112),s__s_Temple_4125_031f,
              (int)DAT_451b_002a + iVar1 * 10 + 0x1000);
      strcpy((char *)CONCAT22(0x3c04,(char *)(iVar3 * 0x1f + 0x815)),
             (char *)CONCAT22(unaff_SS,local_112));
      pcVar6 = (char *)s___03d_The__s_can_bless_your_armi_4125_0329;
    }
    else {
      local_8 = dice(1,*(int *)((int)DAT_451b_002a + 0xed2),-1);
      iVar1 = dice(1,10,-1);
      uVar7 = CONCAT22((int)((ulong)DAT_451b_002a >> 0x10),(int)DAT_451b_002a + iVar1 * 10 + 0xf9c);
      iVar1 = dice(1,10,-1);
      sprintf((char *)CONCAT22(unaff_SS,local_12),(char *)0x412502e0,
              (int)DAT_451b_002a + iVar1 * 10 + 0xf38,DAT_451b_002a._2_2_,uVar7);
      sprintf((char *)CONCAT22(unaff_SS,local_112),
              (char *)CONCAT22(DAT_451b_002a._2_2_,
                               (char *)((int)DAT_451b_002a + local_8 * 0x14 + 0xed4)),local_12);
      strcpy((char *)CONCAT22(0x3c04,(char *)(iVar3 * 0x1f + 0x815)),
             (char *)CONCAT22(unaff_SS,local_112));
      pcVar6 = (char *)s___03d__s_is_inhabited_by_monster_4125_02e5;
    }
    sprintf((char *)CONCAT22(unaff_SS,local_112),(char *)CONCAT22(0x4125,pcVar6),iVar3,
            CONCAT22(0x3c04,iVar3 * 0x1f + 0x815));
    uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    strcpy((char *)CONCAT22(uVar5,(char *)s_print_scanf___floating_point_for_4125_3e8a + 0xc +
                                  (int)DAT_451b_002a + *(int *)((int)DAT_451b_002a + 0x1066)),
           (char *)CONCAT22(unaff_SS,local_112));
    uVar2 = strlen((char *)CONCAT22(unaff_SS,local_112));
    uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    *(int *)((int)DAT_451b_002a + 0x1066) = *(int *)((int)DAT_451b_002a + 0x1066) + uVar2;
  }
  return;
}


