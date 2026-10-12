
void __cdecl16far city_raze(void)

{
  int *piVar1;
  int iVar2;
  undefined2 unaff_SS;
  char local_52 [80];
  
  sprintf((char *)CONCAT22(unaff_SS,local_52),s__s_is_in_ruins__4125_0a56,
          DAT_4125_0a3c * 0x41 + 0x1581);
  FUN_8065_10fb(local_52);
  FUN_8611_006c();
  city_make_ruins(*(int *)(DAT_4125_0a3c * 0x41 + 0x157d),*(int *)(DAT_4125_0a3c * 0x41 + 0x157f));
  FUN_54f6_069c();
  FUN_8065_011f();
  DAT_4125_0a3e = 0;
  iVar2 = dice(1,0xf,0);
  piVar1 = (int *)(current_player * 2 + 0x10e3);
  *piVar1 = *piVar1 + iVar2 + 10;
  quest_check();
  return;
}


