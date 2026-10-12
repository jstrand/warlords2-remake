
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_file_area_dat(void)

{
  int iVar1;
  undefined2 in_DX;
  void *pvVar2;
  uint unaff_DI;
  void *unaff_SS;
  long lVar3;
  byte local_38;
  uint local_14;
  uint local_8;
  int local_6;
  int local_4;
  
  FUN_7ecb_0a1b();
  iVar1 = open(s_DATA_AREA_DAT_4125_16a8,2);
  if (iVar1 < 0) {
    auto_str_fatal_error((char *)s_Area_file_not_found_4125_16b6,0x4125);
  }
  local_8 = 0;
  for (local_4 = 0; local_4 < 7; local_4 = local_4 + 1) {
    read(iVar1,(void *)CONCAT22(0xe,unaff_SS),unaff_DI);
    if (local_8 < local_14) {
      local_8 = local_14;
    }
    for (local_6 = 0; local_6 < (int)local_14; local_6 = local_6 + 1) {
      read(iVar1,(void *)CONCAT22(0xe,unaff_SS),unaff_DI);
    }
  }
  close(iVar1);
  DAT_4125_168e = FUN_1d28_01d5(local_8 * 0xe);
  _DAT_4125_1690 = in_DX;
  iVar1 = open(s_DATA_BUTTON_DAT_4125_16ca,2);
  if (iVar1 < 0) {
    auto_str_fatal_error((char *)s_Button_file_not_found_4125_16da,0x4125);
  }
  for (local_4 = 0; local_4 < 0x24; local_4 = local_4 + 1) {
    read(iVar1,(void *)CONCAT22(0x21,unaff_SS),unaff_DI);
    if ((int)local_8 < (int)(uint)local_38) {
      local_8 = (uint)local_38;
    }
    for (local_6 = 0; local_6 < (int)(uint)local_38; local_6 = local_6 + 1) {
      read(iVar1,(void *)CONCAT22(0x21,unaff_SS),unaff_DI);
    }
  }
  close(iVar1);
  DAT_4125_168a = FUN_1d28_01d5(local_8 * 0x21);
  _DAT_4125_168c = in_DX;
  FUN_7ecb_0a1b();
  iVar1 = open(s_DATA_AREA_DAT_4125_16f0,2);
  if (iVar1 < 0) {
    auto_str_fatal_error((char *)s_Area_file_not_found_4125_16fe,0x4125);
  }
  lVar3 = filelength(iVar1);
  pvVar2 = (void *)((ulong)lVar3 >> 0x10);
  DAT_451b_0ce8._0_2_ = mem_alloc((long)(int)lVar3,0,0x85,0);
  DAT_451b_0ce8._2_2_ = pvVar2;
  mem_lock((undefined2)DAT_451b_0ce8,pvVar2);
  read(iVar1,(void *)CONCAT22((int)lVar3,pvVar2),unaff_DI);
  mem_unlock((undefined2)DAT_451b_0ce8,DAT_451b_0ce8._2_2_);
  close(iVar1);
  iVar1 = open(s_DATA_JOIN_DAT_4125_1712,2);
  if (iVar1 < 0) {
    auto_str_fatal_error((char *)s_Join_file_not_found_4125_1720,0x4125);
  }
  lVar3 = filelength(iVar1);
  pvVar2 = (void *)((ulong)lVar3 >> 0x10);
  DAT_451b_0cec._0_2_ = mem_alloc((long)(int)lVar3,0,0x86,0);
  DAT_451b_0cec._2_2_ = pvVar2;
  mem_lock((undefined2)DAT_451b_0cec,pvVar2);
  read(iVar1,(void *)CONCAT22((int)lVar3,pvVar2),unaff_DI);
  mem_unlock((undefined2)DAT_451b_0cec,DAT_451b_0cec._2_2_);
  close(iVar1);
  iVar1 = open(s_DATA_BUTTON_DAT_4125_1734,2);
  if (iVar1 < 0) {
    auto_str_fatal_error((char *)s_Button_file_not_found_4125_1744,0x4125);
  }
  lVar3 = filelength(iVar1);
  pvVar2 = (void *)((ulong)lVar3 >> 0x10);
  DAT_451b_0ce4._0_2_ = mem_alloc((long)(int)lVar3,0,0x87,0);
  DAT_451b_0ce4._2_2_ = pvVar2;
  mem_lock((undefined2)DAT_451b_0ce4,pvVar2);
  read(iVar1,(void *)CONCAT22((int)lVar3,pvVar2),unaff_DI);
  mem_unlock((undefined2)DAT_451b_0ce4,DAT_451b_0ce4._2_2_);
  close(iVar1);
  FUN_7ecb_0a1b();
  return;
}


