
void __cdecl16far auto_ui_setting_up_game(void)

{
  char *pcVar1;
  char *pcVar2;
  char *pcVar3;
  char *pcVar4;
  int local_6;
  int local_4;
  
  DAT_4125_2adc = FUN_54f6_08bf();
  if (DAT_4125_2adc == 0) {
    FUN_54f6_0000();
  }
  else {
    FUN_54f6_08f6(&local_4);
    FUN_54f6_070b(200 - local_4,0xaf - local_6,0x6900f8);
  }
  get_string(0x8a,6);
  get_string(0x8a,5);
  get_string(0x8a,4);
  pcVar1 = get_string(0x8a,3);
  pcVar2 = get_string(0x8a,2);
  pcVar3 = get_string(0x8a,1);
  pcVar4 = get_string(0x8a,0);
  FUN_8065_1471((char *)pcVar4,(int)((ulong)pcVar4 >> 0x10),pcVar3,pcVar2,pcVar1);
  FUN_4bed_01b5();
  random_map_progress();
  return;
}


