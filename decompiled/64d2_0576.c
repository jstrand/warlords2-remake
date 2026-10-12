
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_file_options_snd(int param_1)

{
  int iVar1;
  uint unaff_SI;
  void *unaff_SS;
  char *name;
  
  if (param_1 == 0) {
    music_disable();
    music_enable();
    _DAT_4125_0bee = (uint)(_DAT_4125_0bee == 0);
    if (_DAT_4125_0bee != 0) {
      music_cue(1);
    }
  }
  else if (param_1 == 2) {
    DAT_4125_26c8 = (uint)(DAT_4125_26c8 == 0);
  }
  else {
    DAT_4125_0bec = (uint)(DAT_4125_0bec == 0);
  }
  ui_side_setup_list(0xfffe,0);
  FUN_64d2_0067();
  iVar1 = 9;
  name = get_file_string(0x44,0);
  iVar1 = open(name,iVar1);
  if (-1 < iVar1) {
    write(iVar1,(void *)CONCAT22(3,unaff_SS),unaff_SI);
    close(iVar1);
  }
  return;
}


