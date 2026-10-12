
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_7087_0978(void)

{
  int iVar1;
  
  iVar1 = 4;
  do {
    _LAB_4125_0e86 = iVar1 + -1;
    if ((iVar1 == 0) || (_LAB_4125_0e86 == 0)) break;
    iVar1 = _LAB_4125_0e86;
  } while ((-1 < *(char *)(DAT_4125_0e98 * 0x41 + _LAB_4125_0e86 + 0x1593)) ||
          (*(char *)(DAT_4125_0e98 * 0x41 + _LAB_4125_0e86 + 0x1592) < '\0'));
  FUN_54f6_0000(0xb);
  auto_ui_build_production(0);
  auto_str_join_data_not_found(0x17);
  FUN_7087_0dae();
  return;
}


