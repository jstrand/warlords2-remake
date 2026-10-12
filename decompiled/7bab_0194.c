
undefined2 __cdecl16far auto_file_x_scn_2(int param_1)

{
  int iVar1;
  uint unaff_DI;
  undefined2 unaff_SS;
  char *pcVar2;
  char *pcVar3;
  char *pcVar4;
  char *pcVar5;
  char *pcVar6;
  char local_66 [60];
  char local_2a [40];
  
  if (param_1 != 0) {
    get_string(0x87,6);
    get_string(0x87,5);
    get_string(0x87,4);
    pcVar5 = get_string(0x87,3);
    pcVar2 = get_string(0x87,2);
    pcVar3 = get_string(0x87,1);
    pcVar4 = get_string(0x87,0);
    FUN_8065_1471((char *)pcVar4,(int)((ulong)pcVar4 >> 0x10),pcVar3,pcVar2,pcVar5);
    FUN_4bed_01b5();
    random_map_progress(0);
  }
  sprintf((char *)CONCAT22(unaff_SS,local_2a),(char *)0x41252565,0x5da0,0x4125,0x5da0,0x4125);
  iVar1 = open((char *)CONCAT22(unaff_SS,local_2a),2);
  if (iVar1 < 0) {
    pcVar6 = local_2a;
    pcVar5 = get_error_string(0,0);
    sprintf((char *)CONCAT22(unaff_SS,local_66),pcVar5,pcVar6);
    auto_str_fatal_error(local_66);
  }
  read(iVar1,(void *)0x2ee13c04,unaff_DI);
  close(iVar1);
  if (param_1 != 0) {
    random_map_progress(0x14);
  }
  load_scenario_map();
  if (param_1 != 0) {
    random_map_progress(0x3c);
  }
  FUN_623c_0000();
  if (param_1 != 0) {
    random_map_progress(100);
    FUN_7ecb_0000(10);
  }
  auto_ui_creating_strategic_map();
  for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
    if (*(int *)(iVar1 * 2 + 0x137) == 0) {
      strcpy((char *)CONCAT22(0x3c04,(char *)(iVar1 * 0x14)),s_Not_used_4125_256f);
      *(undefined2 *)(iVar1 * 2 + 0xc0) = 3;
    }
  }
  return 1;
}


