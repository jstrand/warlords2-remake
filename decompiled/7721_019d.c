
void __cdecl16far auto_ui_new_game(void)

{
  char *pcVar1;
  char *pcVar2;
  char *pcVar3;
  char *pcVar4;
  
  get_string(0x2d,4);
  pcVar1 = get_string(0x2d,3);
  pcVar2 = get_string(0x2d,2);
  pcVar3 = get_string(0x2d,1);
  pcVar4 = get_string(0x2d,0);
  FUN_7b4c_0088((char *)pcVar4,(int)((ulong)pcVar4 >> 0x10),pcVar3,pcVar2,pcVar1);
  return;
}


