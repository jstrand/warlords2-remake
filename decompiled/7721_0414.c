
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __cdecl16far auto_file_save_any_dat(int param_1,int param_2,int param_3)

{
  int iVar1;
  undefined2 uVar2;
  void *pvVar3;
  uint unaff_DI;
  void *unaff_SS;
  char *pcVar4;
  char *pcVar5;
  char *pcVar6;
  char *pcVar7;
  int iVar8;
  char local_2e [20];
  undefined2 local_1a;
  char local_18 [10];
  undefined2 local_e;
  void *pvStack_c;
  undefined2 local_a;
  long local_6;
  
  local_1a = 0;
  if ((DAT_4125_28c6 == 0) && (DAT_4125_2a86 != 0)) {
    pcVar7 = (char *)0x5da8;
  }
  else {
    pcVar7 = (char *)0x129f;
  }
  strcpy((char *)CONCAT22(unaff_SS,local_18),(char *)CONCAT22(0x4125,pcVar7));
  FUN_1997_0066();
  FUN_8611_1927();
  FUN_8611_1931();
  for (local_6 = 0; local_6 < 10; local_6 = local_6 + 1) {
    *(undefined1 *)((int)local_6 * 0x16 + -0x777b) = 0xf;
    *(undefined2 *)((int)local_6 * 0x16 + -0x777e) = 0xffff;
    *(undefined2 *)((int)local_6 * 0x16 + -0x7780) = 0xffff;
  }
  _DAT_451b_1efe = 0;
  DAT_451b_1f02 = 0;
  for (local_6 = 0; local_6 < 8; local_6 = local_6 + 1) {
    *(undefined2 *)((undefined *)&DAT_451b_1ee0 + (int)local_6 * 4) = 0;
    ((undefined2 *)&DAT_451b_1ede)[(int)local_6 * 2] = 0;
  }
  FUN_54f6_0000(0x17);
  if (param_2 == 0) {
    get_string(0x86,6);
    get_string(0x86,5);
    get_string(0x86,4);
    get_string(0x86,3);
    pcVar4 = get_string(0x86,2);
    pcVar5 = get_string(0x86,1);
    iVar8 = 0x86;
  }
  else {
    get_string(0x85,6);
    get_string(0x85,5);
    get_string(0x85,4);
    get_string(0x85,3);
    pcVar4 = get_string(0x85,2);
    pcVar5 = get_string(0x85,1);
    iVar8 = 0x85;
  }
  pcVar6 = get_string(iVar8,0);
  FUN_8065_1471((char *)pcVar6,(int)((ulong)pcVar6 >> 0x10),pcVar5,pcVar4);
  FUN_4bed_01b5();
  random_map_progress(0);
  FUN_8611_113b();
  random_map_progress(10);
  if (param_2 == 0) {
    iVar8 = 0x1a;
  }
  else {
    iVar8 = 0x1c;
  }
  pcVar5 = get_file_string(iVar8,param_1);
  pvVar3 = (void *)((ulong)pcVar5 >> 0x10);
  sprintf((char *)CONCAT22(unaff_SS,local_2e),pcVar5);
  FUN_22bf_07a5();
  iVar8 = open((char *)CONCAT22(unaff_SS,local_2e),2);
  if (-1 < iVar8) {
    read(iVar8,(void *)0x544125,unaff_DI);
    read(iVar8,(void *)0x2ee13c04,unaff_DI);
    random_map_progress(0x14);
    read(iVar8,(void *)0xde702e1d,unaff_DI);
    read(iVar8,(void *)0x444029d9,unaff_DI);
    random_map_progress(0x1e);
    _LAB_4125_126b_1 = mem_lock((int)_LAB_4125_126f_1,(int)((ulong)_LAB_4125_126f_1 >> 0x10));
    _LAB_4125_126d_1 = pvVar3;
    read(iVar8,(void *)CONCAT22((undefined2 *)&DAT_451b_04e0,pvVar3),unaff_DI);
    mem_unlock((int)_LAB_4125_126f_1,(int)((ulong)_LAB_4125_126f_1 >> 0x10));
    random_map_progress(0x28);
    read(iVar8,(void *)CONCAT22(4,unaff_SS),unaff_DI);
    local_e = FUN_1d28_01d5(local_a);
    pvStack_c = pvVar3;
    read(iVar8,(void *)CONCAT22(local_a,pvVar3),unaff_DI);
    iVar1 = 9;
    pcVar5 = get_file_string(0x1d,0);
    pvVar3 = (void *)((ulong)pcVar5 >> 0x10);
    iVar1 = open(pcVar5,iVar1);
    write(iVar1,(void *)CONCAT22(local_a,pvStack_c),unaff_DI);
    close(iVar1);
    FUN_1d28_050d(local_e,pvStack_c);
    random_map_progress(0x32);
    if (DAT_3c04_0120 != 0) {
      read(iVar8,(void *)CONCAT22(4,unaff_SS),unaff_DI);
      local_e = FUN_1d28_01d5(local_a);
      pvStack_c = pvVar3;
      read(iVar8,(void *)CONCAT22(local_a,pvVar3),unaff_DI);
      iVar1 = 9;
      pcVar5 = get_file_string(0x22,0);
      pvVar3 = (void *)((ulong)pcVar5 >> 0x10);
      iVar1 = open(pcVar5,iVar1);
      write(iVar1,(void *)CONCAT22(local_a,pvStack_c),unaff_DI);
      close(iVar1);
      FUN_1d28_050d(local_e,pvStack_c);
    }
    random_map_progress(0x3c);
    if (DAT_3c04_0120 != 0) {
      read(iVar8,(void *)CONCAT22(4,unaff_SS),unaff_DI);
      local_e = FUN_1d28_01d5(local_a);
      pvStack_c = pvVar3;
      read(iVar8,(void *)CONCAT22(local_a,pvVar3),unaff_DI);
      iVar1 = 9;
      pcVar5 = get_file_string(0x23,0);
      pvVar3 = (void *)((ulong)pcVar5 >> 0x10);
      iVar1 = open(pcVar5,iVar1);
      write(iVar1,(void *)CONCAT22(local_a,pvStack_c),unaff_DI);
      close(iVar1);
      FUN_1d28_050d(local_e,pvStack_c);
    }
    random_map_progress(0x46);
    read(iVar8,(void *)CONCAT22(4,unaff_SS),unaff_DI);
    uVar2 = FUN_1d28_01d5(local_a);
    _local_e = CONCAT22(pvVar3,uVar2);
    read(iVar8,(void *)CONCAT22(local_a,pvVar3),unaff_DI);
    iVar1 = 9;
    pcVar5 = get_file_string(6,0);
    iVar1 = open(pcVar5,iVar1);
    write(iVar1,(void *)CONCAT22(local_a,(void *)((ulong)_local_e >> 0x10)),unaff_DI);
    close(iVar1);
    FUN_1d28_050d((int)_local_e,(int)((ulong)_local_e >> 0x10));
    DAT_3c04_0130 = 1;
    if (param_2 != 0) {
      FUN_623c_0dff(iVar8);
    }
    close(iVar8);
  }
  random_map_progress(0x50);
  if (opt_hidden_map != 0) {
    FUN_834b_2200(current_player);
  }
  FUN_1997_034a(7);
  random_map_progress(0x5a);
  if ((param_3 != 0) ||
     (iVar8 = strcmp((char *)0x41255da8,(char *)CONCAT22(unaff_SS,local_18)), iVar8 != 0)) {
    auto_ui_creating_strategic_map();
    local_1a = 1;
  }
  if (DAT_3c04_0120 != 0) {
    FUN_7204_1de4();
  }
  random_map_progress(100);
  FUN_54f6_069c();
  auto_file_standard_pal(1);
  FUN_22bf_0757();
  return local_1a;
}


