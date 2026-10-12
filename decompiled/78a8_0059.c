
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far
auto_file_x_fnt(undefined4 param_1,undefined4 param_2,undefined4 param_3,long param_4,
               undefined4 *param_5)

{
  undefined4 uVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined2 uVar5;
  undefined2 extraout_DX;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  undefined2 unaff_SS;
  char *pcVar9;
  int iVar10;
  undefined2 uVar11;
  int iVar12;
  undefined2 uVar13;
  char local_ea [14];
  char local_dc [200];
  undefined1 local_14 [8];
  undefined2 local_c;
  undefined2 local_a;
  undefined2 local_8;
  undefined4 local_6;
  
  local_8 = 1;
  pcVar9 = strcpy((char *)CONCAT22(unaff_SS,local_dc),(char *)0x4125147e);
  uVar5 = (undefined2)((ulong)pcVar9 >> 0x10);
  uVar11 = (undefined2)((ulong)param_1 >> 0x10);
  sprintf((char *)CONCAT22(unaff_SS,local_ea),(char *)0x4125147f,(int)param_1,uVar11);
  iVar7 = (int)param_3;
  uVar4 = (undefined2)((ulong)param_3 >> 0x10);
  iVar2 = auto_str_cannot_allocate_this_newbm(iVar7,uVar4,local_dc);
  if (iVar2 == 0) {
    sprintf((char *)CONCAT22(unaff_SS,local_dc),s_Couldn_t_load_the_file___s___4125_1486,local_ea);
    auto_str_fatal_error(local_dc);
  }
  sprintf((char *)CONCAT22(unaff_SS,local_ea),s__s_fin_4125_14a3,(int)param_1,uVar11);
  iVar10 = (int)param_2;
  uVar11 = (undefined2)((ulong)param_2 >> 0x10);
  iVar2 = FUN_78a8_03ea(iVar10,uVar11,iVar7,uVar4,local_dc);
  if (iVar2 == 0) {
    sprintf((char *)CONCAT22(unaff_SS,local_dc),s_Couldn_t_load_the_file___s___4125_14aa,local_ea);
    auto_str_fatal_error(local_dc);
  }
  if (param_4 != 0) {
    local_a = *(undefined2 *)(iVar10 + 2);
    local_c = *(undefined2 *)(iVar10 + 0xd);
    _LAB_4125_12d1_1 = 0;
    iVar2 = FUN_2012_01fe(0x12ca,0x4125,local_a,local_c);
    if (iVar2 == 0) {
      auto_str_fatal_error((char *)s_Couldn_t_allocate_the_temp_mask_b_4125_14c7,0x4125);
    }
    iVar12 = (int)param_4;
    uVar13 = (undefined2)((ulong)param_4 >> 0x10);
    uVar8 = 0x2012;
    iVar2 = FUN_2012_0232(iVar12,uVar13,local_a,local_c,1);
    if (iVar2 == 0) {
      uVar8 = 0x79de;
      auto_str_fatal_error(0x14f0,0x4125);
    }
    if ((_LAB_4125_12d3_1 != 0) && ((DAT_4125_12ff & 10) == 0)) {
      for (iVar2 = 0; iVar2 < (int)(uint)DAT_4125_12fe; iVar2 = iVar2 + 1) {
        uVar8 = 0x1d28;
        uVar3 = mem_lock((int)*(undefined4 *)((undefined1 *)&LAB_4125_12d3_1 + iVar2 * 4),
                         (int)((ulong)*(undefined4 *)((undefined1 *)&LAB_4125_12d3_1 + iVar2 * 4) >>
                              0x10));
        *(undefined2 *)((undefined1 *)&LAB_4125_12e5_1 + iVar2 * 4) = uVar5;
        *(undefined2 *)((undefined1 *)&LAB_4125_12e3_1 + iVar2 * 4) = uVar3;
      }
    }
    if ((*(long *)(iVar12 + 10) != 0) && ((*(uint *)(iVar12 + 0x35) & 10) == 0)) {
      for (iVar2 = 0; iVar2 < (int)(uint)*(byte *)(iVar12 + 0x34); iVar2 = iVar2 + 1) {
        uVar1 = *(undefined4 *)(iVar12 + iVar2 * 4 + 10);
        uVar8 = 0x1d28;
        uVar3 = mem_lock((int)uVar1,(int)((ulong)uVar1 >> 0x10));
        iVar6 = iVar12 + iVar2 * 4;
        *(undefined2 *)(iVar6 + 0x1c) = uVar5;
        *(undefined2 *)(iVar6 + 0x1a) = uVar3;
      }
    }
    if ((*(long *)(iVar7 + 10) != 0) && ((*(uint *)(iVar7 + 0x35) & 10) == 0)) {
      for (iVar2 = 0; iVar2 < (int)(uint)*(byte *)(iVar7 + 0x34); iVar2 = iVar2 + 1) {
        uVar1 = *(undefined4 *)(iVar7 + iVar2 * 4 + 10);
        uVar8 = 0x1d28;
        uVar3 = mem_lock((int)uVar1,(int)((ulong)uVar1 >> 0x10));
        iVar6 = iVar7 + iVar2 * 4;
        *(undefined2 *)(iVar6 + 0x1c) = uVar5;
        *(undefined2 *)(iVar6 + 0x1a) = uVar3;
      }
    }
    (*DAT_451b_2a74)(uVar8,param_3,0,0,local_a,local_c,0x12ca,0x4125,0,0);
    uVar5 = extraout_DX;
    auto_str_cannot_allocate_mask(&local_8);
    if ((*(long *)(iVar12 + 10) != 0) && ((*(uint *)(iVar12 + 0x35) & 10) == 0)) {
      for (iVar2 = 0; iVar2 < (int)(uint)*(byte *)(iVar12 + 0x34); iVar2 = iVar2 + 1) {
        uVar1 = *(undefined4 *)(iVar12 + iVar2 * 4 + 10);
        mem_unlock((int)uVar1,(int)((ulong)uVar1 >> 0x10));
      }
    }
    if ((*(long *)(iVar7 + 10) != 0) && ((*(uint *)(iVar7 + 0x35) & 10) == 0)) {
      for (iVar2 = 0; iVar2 < (int)(uint)*(byte *)(iVar7 + 0x34); iVar2 = iVar2 + 1) {
        uVar1 = *(undefined4 *)(iVar7 + iVar2 * 4 + 10);
        mem_unlock((int)uVar1,(int)((ulong)uVar1 >> 0x10));
      }
    }
    FUN_2012_0975(0x12ca,0x4125);
  }
  iVar2 = *(int *)(iVar10 + 0xf);
  uVar4 = mem_alloc(iVar2 * 0x2a,0,0x20,0,0);
  uVar4 = FUN_1d28_075d(uVar4,uVar5);
  local_6 = CONCAT22(uVar5,uVar4);
  for (iVar7 = 0; iVar7 < iVar2; iVar7 = iVar7 + 1) {
    FUN_216d_002e(local_14);
    auto_str_font_can_t_create((int)local_6 + iVar7 * 0x2a,local_6._2_2_,param_3,param_4,local_14);
  }
  *param_5 = local_6;
  return;
}


