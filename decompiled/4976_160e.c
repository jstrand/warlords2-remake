
void __cdecl16far auto_ui_quest_reward_allies(uint param_1,int param_2)

{
  char *pcVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 unaff_SS;
  char *pcVar4;
  undefined4 uVar5;
  char local_5e [80];
  undefined1 local_e [4];
  int local_a;
  
  FUN_78a8_06ae(2,0,0x10007);
  __F_SCOPY(current_player * 0xc + 0x1103,0x3c04,local_e,unaff_SS);
  sprintf((char *)CONCAT22(unaff_SS,local_5e),s__s_s_Quest_4125_00ce,
          *(char *)(local_a * 0x16 + -0x7776) * 0x14 + 0x223,0x3c04);
  FUN_7ecb_00d6(0x1b8,0x91,local_5e);
  FUN_24d0_0002();
  FUN_2133_02fe(0x168,0xa000a5);
  pcVar4 = get_string(0x1c,0);
  FUN_7ecb_00d6(0x1b8,0xaf,(char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
  pcVar4 = get_string(0x1c,1);
  FUN_7ecb_00d6(0x1b8,0xc3,(char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
  if (3 < param_1) {
    return;
  }
                    /* WARNING: Switch is manually overridden */
  switch((undefined4)*(undefined2 *)(param_1 * 2 + 0x1899)) {
  case 0x4ae4d:
    pcVar4 = get_string(0x1d,0);
    FUN_7ecb_00d6(0x1b8,0xe1,(char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
    pcVar4 = get_string(0x1d,1);
    FUN_7ecb_00d6(0x1b8,0xf5,(char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
    FUN_7ecb_00d6(0x1b8,0x109,param_2 * 0x1f + 0x815,0x3c04);
    FUN_828e_0a3f(*(undefined2 *)(local_a * 0x16 + -0x7780),
                  *(undefined2 *)(local_a * 0x16 + -0x777e),*(undefined2 *)(param_2 * 0x1f + 0x811),
                  *(undefined2 *)(param_2 * 0x1f + 0x813),8);
    FUN_828e_0806(param_2,8);
    FUN_828e_0868(param_2,8);
    return;
  case 0x4af02:
    pcVar4 = get_string(0x1e,0);
    FUN_7ecb_00d6(0x1b8,0xe1,(char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
    pcVar4 = get_string(0x1e,1);
    FUN_7ecb_00d6(0x1b8,0xf5,(char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
    pcVar1 = (char *)(param_2 * 0x1d + 0xce9);
    unaff_SS = 0x3c04;
    goto LAB_4976_188a;
  case 0x4af48:
    pcVar4 = get_string(0x1f,0);
    FUN_7ecb_00d6(0x1b8,0xe1,(char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
    pcVar4 = get_string(0x1f,1);
    uVar3 = (undefined2)((ulong)pcVar4 >> 0x10);
    FUN_7ecb_00d6(0x1b8,0xf5,pcVar4);
    uVar2 = FUN_7087_14f4();
    uVar5 = CONCAT22(uVar3,uVar2);
    break;
  case 0x4af8b:
    pcVar4 = get_string(0x1f,0);
    FUN_7ecb_00d6(0x1b8,0xe1,(char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
    pcVar4 = get_string(0x1f,1);
    FUN_7ecb_00d6(0x1b8,0xf5,(char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
    uVar5 = 0x412500d9;
  }
  pcVar4 = get_string(0x1f,2);
  sprintf((char *)CONCAT22(unaff_SS,local_5e),pcVar4,param_2,uVar5);
  pcVar1 = local_5e;
LAB_4976_188a:
  FUN_7ecb_00d6(0x1b8,0x109,pcVar1,unaff_SS);
  return;
}


