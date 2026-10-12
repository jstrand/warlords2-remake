
void __cdecl16far auto_err_error_cannot_open_file(int param_1)

{
  int iVar1;
  int index;
  int group;
  undefined2 uVar2;
  undefined2 unaff_SS;
  char *pcVar3;
  char *fmt;
  undefined1 local_8e [52];
  undefined1 local_5a;
  char local_52 [80];
  
  if (param_1 == 0) {
    group = 0x41;
  }
  else {
    group = 0x42;
  }
  FUN_22bf_07a5();
  for (index = 0; index < 4; index = index + 1) {
    local_5a = 0;
    get_file_string(group,index);
    uVar2 = 0x197b;
    iVar1 = auto_str_cannot_allocate_this_newbm(local_8e);
    if (iVar1 == 0) {
      pcVar3 = get_file_string(group,index);
      fmt = get_error_string(1,0);
      sprintf((char *)CONCAT22(unaff_SS,local_52),fmt,(char *)pcVar3,(int)((ulong)pcVar3 >> 0x10));
      uVar2 = 0x79de;
      auto_str_fatal_error(local_52);
    }
    (*DAT_451b_2a74)(uVar2,local_8e);
    FUN_2012_0975(local_8e);
  }
  FUN_22bf_0757();
  return;
}


