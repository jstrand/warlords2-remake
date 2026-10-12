
void __cdecl16far random_map_setup(void)

{
  int iVar1;
  int iVar2;
  undefined2 unaff_SS;
  char *pcVar3;
  char *pcVar4;
  char *pcVar5;
  char *pcVar6;
  char local_14 [6];
  undefined1 local_e;
  int local_c;
  undefined4 local_a;
  undefined4 local_6;
  
  for (iVar2 = 0; iVar2 < 6; iVar2 = iVar2 + 1) {
    local_c = dice(1,0x5a,0);
    local_c = local_c + 0x20;
    local_14[iVar2] = (char)local_c;
  }
  local_e = 0;
  get_string(0x88,6);
  get_string(0x88,5);
  get_string(0x88,4);
  pcVar3 = get_string(0x88,3);
  pcVar4 = get_string(0x88,2);
  pcVar5 = get_string(0x88,1);
  pcVar6 = get_string(0x88,0);
  FUN_8065_1471((char *)pcVar6,(int)((ulong)pcVar6 >> 0x10),pcVar5,pcVar4,pcVar3);
  FUN_22bf_07a5();
  for (iVar2 = 0; iVar2 < 4; iVar2 = iVar2 + 1) {
    if (((undefined2 *)&DAT_4125_28d0)[iVar2] == 7) {
      iVar1 = dice(1,7,-1);
    }
    else {
      iVar1 = ((undefined2 *)&DAT_4125_28d0)[iVar2];
    }
    *(int *)((int)&local_a + iVar2 * 2) = iVar1;
  }
  random_map_load_params(local_a,local_6);
  strcpy((char *)0x41255da0,s_random_4125_255c);
  strcpy((char *)0x41255d78,s_random_4125_255c);
  strcpy((char *)0x41255da8,(char *)CONCAT22(unaff_SS,local_14));
  DAT_3c04_0161 = DAT_4125_28ca;
  strcpy(s_Illuria__4125_1672,(char *)0x4125259d);
  auto_file_x_scn_2();
  auto_file_rmaptemp_dat();
  FUN_22bf_0757();
  return;
}


