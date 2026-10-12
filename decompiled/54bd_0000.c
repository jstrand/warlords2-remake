
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_file_warlord2_hlp(int param_1)

{
  int fd;
  uint unaff_DI;
  int iVar1;
  void *unaff_SS;
  int local_72;
  int local_70;
  undefined1 local_6e [100];
  undefined1 local_a [4];
  int local_6;
  int local_4;
  
  local_6 = 0;
  if (_LAB_4125_1174 == 0) {
    fd = open(s_help_warlord2_hlp_4125_05c6,2);
    if (fd < 0) {
      printf(s_No_help_file_4125_05d8);
    }
    else {
      read(fd,(void *)CONCAT22(2,unaff_SS),unaff_DI);
      iVar1 = 0;
      do {
        read(fd,(void *)CONCAT22(0x68,unaff_SS),unaff_DI);
        iVar1 = iVar1 + 1;
        if (local_72 == param_1) {
          local_6 = 1;
          break;
        }
      } while (iVar1 < local_4);
      close(fd);
      if (local_6 != 0) {
        if (local_70 == 0) {
          FUN_22bf_0af4(local_a);
          FUN_740d_1201(local_a._0_2_,local_a._2_2_,0);
          FUN_740d_1158(local_6e);
          _LAB_4125_1174 = 1;
        }
        else {
          FUN_22bf_0af4(local_a);
          auto_str_user_defined_button(local_70,local_a._0_2_,local_a._2_2_);
        }
      }
    }
  }
  return;
}


