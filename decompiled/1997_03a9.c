
void __cdecl16far auto_file_files_any_pck(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  undefined2 unaff_SS;
  char *pcVar3;
  char *fmt;
  undefined1 local_a8 [52];
  undefined1 local_74;
  undefined2 local_73;
  char local_6c [50];
  char local_3a [50];
  uint local_8;
  undefined1 *local_6;
  undefined2 uVar4;
  
  strcpy((char *)CONCAT22(unaff_SS,local_6c),(char *)0x4125230c);
  if ((((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] & 3) == 1) {
    sprintf((char *)CONCAT22(unaff_SS,local_6c),(char *)0x4125230d,0x5da8,0x4125);
  }
  else if ((((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] & 3) == 2) {
    sprintf((char *)CONCAT22(unaff_SS,local_6c),s_terrain_d__4125_2316,DAT_3c04_0161);
  }
  else {
    strcpy((char *)CONCAT22(unaff_SS,local_6c),(char *)0x41252321);
  }
  if ((((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] & 0x10) == 0) {
    uVar4 = *(undefined2 *)(param_1 * 0xc + 0x1f59);
    local_6 = (undefined1 *)*(int *)(param_1 * 0xc + 0x1f57);
  }
  else {
    local_74 = 0;
    local_73 = 0;
    local_6 = local_a8;
    uVar4 = unaff_SS;
  }
  get_file_string(3,param_1);
  iVar2 = auto_str_cannot_allocate_this_newbm(local_6,uVar4,local_6c);
  if (iVar2 == 0) {
    pcVar3 = get_file_string(3,param_1);
    fmt = get_error_string(1,0);
    sprintf((char *)CONCAT22(unaff_SS,local_3a),fmt,(char *)pcVar3,(int)((ulong)pcVar3 >> 0x10));
    auto_str_fatal_error(local_3a);
  }
  ((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] = ((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] | 4;
  if ((((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] & 0x20) != 0) {
    local_8 = ((byte)((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] & 0x40) >> 6;
    auto_str_cannot_allocate_mask();
    ((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] =
         ((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] & 0xbf;
    ((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] =
         ((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] | ((byte)local_8 & 1) << 6;
  }
  if ((((undefined1 *)&DAT_4125_1f56)[param_1 * 0xc] & 0x10) != 0) {
    uVar1 = *(undefined4 *)(param_1 * 0xc + 0x1f57);
    FUN_2012_09cf((int)uVar1,(int)((ulong)uVar1 >> 0x10));
    uVar1 = *(undefined4 *)(param_1 * 0xc + 0x1f57);
    FUN_2012_01fe((int)uVar1,(int)((ulong)uVar1 >> 0x10),(int)local_a8._4_4_,SUB42(local_a8._4_4_,2)
                 );
    (*DAT_451b_2a74)(0x2012,local_a8);
    FUN_2012_0975(local_a8);
  }
  return;
}


