
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far sage_visit(undefined2 param_1)

{
  int iVar1;
  char *fmt;
  undefined2 uVar2;
  
  FUN_22bf_07a5();
  sage_build_offer_list();
  music_cue(6);
  FUN_6536_175b(0xffff);
  iVar1 = *(char *)((int)_DAT_451b_1efe + 10) * 0x14 + 0x223;
  uVar2 = 0x3c04;
  fmt = get_string(0x3a,0);
  sprintf(&DAT_451b_1cd8,fmt,iVar1,uVar2);
  FUN_6536_17c9((undefined1 *)&DAT_451b_1cd8,0x4125);
  sound_dramatic(0);
  FUN_7ecb_0000(10);
  FUN_54f6_069c();
  auto_ui_sage_greeting(param_1);
  FUN_22bf_0757();
  return;
}


