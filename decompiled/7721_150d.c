
void __cdecl16far auto_ui_resign(void)

{
  char *pcVar1;
  
  if (((undefined2 *)&side_city_count)[current_player] != 0) {
    DAT_4125_166a = 1;
    FUN_54f6_0000();
    FUN_78a8_06ae(0xf0001,0x30000);
    pcVar1 = get_string(0xa5,0);
    FUN_7ecb_00d6(0x5c0140,(char *)pcVar1,(int)((ulong)pcVar1 >> 0x10));
    FUN_78a8_06ae(0xf0002,0x30000);
    pcVar1 = get_string(0xa5,1);
    FUN_7ecb_00d6(0x8c0140,(char *)pcVar1,(int)((ulong)pcVar1 >> 0x10));
    pcVar1 = get_string(0xa5,2);
    FUN_7ecb_00d6(0xa00140,(char *)pcVar1,(int)((ulong)pcVar1 >> 0x10));
    pcVar1 = get_string(0xa5,3);
    FUN_7ecb_00d6(0xb40140,(char *)pcVar1,(int)((ulong)pcVar1 >> 0x10));
    auto_str_join_data_not_found();
    FUN_18a9_066e(0x101e7);
    FUN_18a9_066e(0x101e8);
    FUN_18a9_066e(0x101e9);
  }
  return;
}


