
void __cdecl16far hero_promote(int param_1,int param_2)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  undefined2 unaff_SS;
  char *pcVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  char local_30 [40];
  undefined2 local_8;
  int local_6;
  int local_4;
  
  local_6 = -(int)*(char *)(param_1 * 0x16 + -0x7778);
  iVar2 = (int)*(char *)(param_1 * 0x16 + -0x7776);
  bVar1 = *(byte *)(iVar2 + 0x5e3);
  *(byte *)(iVar2 + 0x5e3) = *(byte *)(iVar2 + 0x5e3) & 0xfc;
  *(byte *)(iVar2 + 0x5e3) = *(byte *)(iVar2 + 0x5e3) | (bVar1 & 3) + 1 & 3;
  army_add_strength();
  local_6 = local_6 + *(char *)(param_1 * 0x16 + -0x7778);
  local_8 = 2;
  *(char *)(param_1 * 0x16 + -0x777a) = *(char *)(param_1 * 0x16 + -0x777a) + '\x02';
  iVar3 = is_computer_turn();
  if (iVar3 == 0) {
    music_cue(7);
    FUN_22bf_07a5();
    FUN_54f6_0000();
    if (*(int *)(iVar2 * 2 + 0x593) == 0) {
      local_4 = 0;
    }
    else {
      local_4 = 6;
    }
    FUN_78a8_06ae(0xf0001,0x30000);
    pcVar4 = get_string(param_2,local_4);
    FUN_7ecb_00d6(0x5d0140,(char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
    FUN_78a8_06ae(0xf0002,0x30000);
    FUN_7ecb_00d6(0x8c0140,iVar2 * 0x14 + 0x223);
    pcVar4 = get_string(param_2,local_4 + 1);
    FUN_7ecb_00d6(0xa00140,(char *)pcVar4);
    pcVar4 = get_string(param_2,local_4 + 2);
    FUN_7ecb_00d6(0xb40140,(char *)pcVar4);
    pcVar4 = get_string(param_2,local_4 + 3);
    FUN_7ecb_00d6(0xc80140,(char *)pcVar4);
    pcVar4 = get_string(param_2,local_4 + 4);
    FUN_7ecb_00d6(0xe80140,(char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
    uVar6 = CONCAT22(local_8,(int)*(char *)(param_1 * 0x16 + -0x777a));
    uVar5 = CONCAT22(local_6,(int)*(char *)(param_1 * 0x16 + -0x7778));
    iVar2 = (*(byte *)(iVar2 + 0x5e3) & 3) + 1;
    pcVar4 = get_string(param_2,local_4 + 5);
    sprintf((char *)CONCAT22(unaff_SS,local_30),pcVar4,iVar2,uVar5,uVar6);
    FUN_7ecb_00d6(0xfc0140,local_30);
    FUN_22bf_0757();
    DAT_4125_166a = 1;
    FUN_7ecb_0142();
    DAT_4125_166a = 0;
    FUN_54f6_069c();
  }
  return;
}


