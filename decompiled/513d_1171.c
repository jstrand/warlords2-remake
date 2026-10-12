
void __cdecl16far random_map_city_economy(int city,int value)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  undefined1 *puVar5;
  undefined2 uVar6;
  undefined2 unaff_SS;
  undefined2 uVar7;
  char *pcVar8;
  char *pcVar9;
  undefined2 uVar10;
  int sides;
  char local_110 [256];
  int local_10;
  undefined2 uStack_e;
  char *local_c;
  undefined2 uStack_a;
  char *local_8;
  int local_4;
  
  FUN_513d_1468(city);
  iVar1 = dice(1,8,0);
  *(int *)(city * 0x41 + 0x15a7) = value * 2 + iVar1 + 0xe;
  iVar1 = dice(1,3,value + -2);
  iVar2 = dice(1,3,value + -2);
  if (iVar1 < 1) {
    iVar1 = 0;
  }
  if (8 < iVar1) {
    iVar1 = 9;
  }
  if (iVar2 < 1) {
    iVar2 = 0;
  }
  if (8 < iVar2) {
    iVar2 = 9;
  }
  if ((((DAT_4125_02d8 == 0) && (DAT_4125_02de == 0)) && (DAT_4125_02dc == 0)) ||
     (iVar3 = dice(1,100,0), 0x4f < iVar3)) {
    iVar3 = dice(1,100,0);
    if (iVar3 < 0x32) {
      iVar3 = dice(1,10,-1);
      local_8 = (char *)CONCAT22((int)((ulong)DAT_451b_002a >> 0x10),
                                 (char *)s_DATA_AREA_DAT_4125_16a8 +
                                 (int)DAT_451b_002a + iVar3 * 0x10);
      iVar3 = dice(1,10,-1);
      local_c = (char *)s_Button_file_not_found_4125_1744 + 4 + (int)DAT_451b_002a + iVar3 * 0x10;
      uStack_a = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      iVar3 = dice(1,10,-1);
      iVar3 = (int)DAT_451b_002a + iVar3 * 0x10 + 0x17e8;
      uVar6 = DAT_451b_002a._2_2_;
    }
    else {
      iVar3 = dice(1,10,-1);
      local_8 = (char *)CONCAT22((int)((ulong)DAT_451b_002a >> 0x10),
                                 (char *)s_Couldn_t_allocate_the_temp_mask_b_4125_14c7 + 1 +
                                 (int)DAT_451b_002a + iVar3 * 0x10);
      iVar3 = dice(1,10,-1);
      local_c = (char *)((int)DAT_451b_002a + iVar3 * 0x10 + 0x1568);
      uStack_a = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      iVar3 = dice(1,10,-1);
      iVar3 = (int)DAT_451b_002a + iVar3 * 0x10 + 0x1608;
      uVar6 = DAT_451b_002a._2_2_;
    }
  }
  else {
    if (DAT_4125_02de == 0) {
      if (DAT_4125_02dc == 0) {
        iVar3 = -1;
      }
      else {
        iVar3 = 3;
      }
      sides = 4;
    }
    else {
      sides = 2;
      iVar3 = 7;
    }
    local_4 = dice(1,sides,iVar3);
    iVar3 = dice(1,10,-1);
    local_8 = (char *)CONCAT22((int)((ulong)DAT_451b_002a >> 0x10),
                               (char *)((int)DAT_451b_002a + iVar3 * 0x10 + 0x12e8));
    iVar3 = dice(1,10,-1);
    local_c = (char *)((int)DAT_451b_002a + iVar3 * 0x10 + 5000);
    uStack_a = DAT_451b_002a._2_2_;
    iVar3 = (int)DAT_451b_002a + local_4 * 0x10 + 0x1428;
    uVar6 = DAT_451b_002a._2_2_;
  }
  _local_10 = CONCAT22(uVar6,iVar3);
  puVar5 = (undefined1 *)&LAB_4125_1247_1 + (int)DAT_451b_002a + iVar2 * 0x10;
  uVar7 = DAT_451b_002a._2_2_;
  pcVar8 = local_8;
  pcVar9 = local_c;
  uVar10 = uStack_a;
  iVar2 = dice(1,3,-1);
  sprintf((char *)CONCAT22(unaff_SS,local_110),s___03d__s_is_a__s__s___s__s__s__4125_0361,city,
          city * 0x41 + 0x1581,0x3c04,(int)DAT_451b_002a + iVar1 * 0x30 + iVar2 * 0x10 + 0x1068,
          (int)((ulong)DAT_451b_002a >> 0x10),puVar5,uVar7,pcVar8,pcVar9,uVar10,iVar3,uVar6);
  uVar6 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
  strcpy((char *)CONCAT22(uVar6,(undefined1 *)&DAT_4125_1f56 +
                                (int)DAT_451b_002a + *(int *)((int)DAT_451b_002a + 0x1064)),
         (char *)CONCAT22(unaff_SS,local_110));
  uVar4 = strlen((char *)CONCAT22(unaff_SS,local_110));
  uVar6 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
  *(int *)((int)DAT_451b_002a + 0x1064) = *(int *)((int)DAT_451b_002a + 0x1064) + uVar4;
  return;
}


