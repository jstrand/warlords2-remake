
void __cdecl16far sage_gem(void)

{
  int *piVar1;
  int iVar2;
  char *pcVar3;
  
  FUN_22bf_07a5();
  iVar2 = dice(3,500,500);
  piVar1 = (int *)(current_player * 0x14 + 0x185);
  *piVar1 = *piVar1 + iVar2;
  pcVar3 = get_string(0x3b,0);
  FUN_6536_15d5(pcVar3);
  pcVar3 = get_string(0x3b,1);
  sprintf(&DAT_451b_1cd8,pcVar3,iVar2);
  FUN_6536_15d5((undefined1 *)&DAT_451b_1cd8,0x4125);
  FUN_54f6_070b(8,0x116,0xf0,0x17);
  FUN_18a9_03b3(0x118);
  FUN_18a9_03b3(0x119);
  FUN_18a9_03b3(0x117);
  FUN_18a9_066e(0x11a,1);
  FUN_22bf_0757();
  return;
}


