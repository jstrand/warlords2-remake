
void __cdecl16far auto_ui_hero_levels(void)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  undefined2 unaff_SS;
  bool bVar5;
  char *pcVar6;
  char *pcVar7;
  int group;
  undefined2 uVar8;
  char local_1e [20];
  int local_a;
  int local_8;
  int local_6;
  uint local_4;
  
  iVar4 = 0;
  FUN_22bf_07a5();
  FUN_54f6_0000(0);
  DAT_4125_166a = 1;
  auto_str_join_data_not_found(0x1c);
  FUN_18a9_066e(0x1d5,1);
  FUN_78a8_06ae(1,0xf,0,3);
  pcVar6 = get_string(0x68,0);
  FUN_7ecb_00d6(0x140,0x3f,pcVar6);
  FUN_78a8_06ae(2,*(undefined2 *)(current_player * 2 + 0xa0),
                *(undefined2 *)(current_player * 2 + 0xb0),3);
  pcVar6 = get_string(0x68,1);
  FUN_7ecb_00bf(0x80,0x69,pcVar6);
  pcVar6 = get_string(0x68,2);
  FUN_7ecb_00bf(0x110,0x69,pcVar6);
  pcVar6 = get_string(0x68,3);
  FUN_7ecb_00d6(0x188,0x69,pcVar6);
  pcVar6 = get_string(0x68,4);
  FUN_7ecb_00d6(0x1b8,0x69,pcVar6);
  pcVar6 = get_string(0x68,5);
  FUN_7ecb_00d6(0x1e8,0x69,pcVar6);
  pcVar6 = get_string(0x68,6);
  FUN_7ecb_00d6(0x218,0x69,pcVar6);
  FUN_78a8_06ae(2,0xf,0,3);
  local_4 = 4;
  do {
    if (local_4 == 0) {
      local_4 = 0xffff;
      local_a = iVar4;
      for (; iVar4 < 6; iVar4 = iVar4 + 1) {
        FUN_8611_08be(0xffff,current_player,0x342a,0x4125,0x58,iVar4 * 0x1e + 0x80,0,0,0,0,1,0,0);
      }
      FUN_22bf_0757();
      return;
    }
    local_6 = army_count;
    local_4 = local_4 - 1;
    while (iVar2 = local_6 + -1, bVar5 = local_6 != 0, local_6 = iVar2, bVar5) {
      if (((*(char *)(iVar2 * 0x16 + -0x777c) == '\x1c') &&
          (*(char *)(iVar2 * 0x16 + -0x777b) == current_player)) &&
         ((*(byte *)(*(char *)(iVar2 * 0x16 + -0x7776) + 0x5e3) & 3) == local_4)) {
        iVar2 = (int)*(char *)(iVar2 * 0x16 + -0x7776);
        if (local_4 == 3) {
          local_8 = current_player + 2;
        }
        else {
          local_8 = 1;
        }
        FUN_8611_08be(0x1c,current_player,0x342a,0x4125,0x58,iVar4 * 0x1e + 0x80,0,0,0,0,local_8,0,0
                     );
        FUN_7ecb_00bf(0x80,iVar4 * 0x1e + 0x86,iVar2 * 0x14 + 0x223,0x3c04);
        if (*(int *)(iVar2 * 2 + 0x593) == 0) {
          bVar1 = *(byte *)(iVar2 + 0x5e3);
          group = 99;
        }
        else {
          bVar1 = *(byte *)(iVar2 + 0x5e3);
          group = 100;
        }
        pcVar6 = get_string(group,bVar1 & 3);
        FUN_7ecb_00bf(0x110,iVar4 * 0x1e + 0x86,pcVar6);
        sprintf((char *)CONCAT22(unaff_SS,local_1e),&LAB_4125_115d,
                (uint)(*(byte *)(iVar2 + 0x5e3) >> 2));
        FUN_7ecb_00d6(0x188,iVar4 * 0x1e + 0x86,local_1e);
        uVar3 = *(byte *)(iVar2 + 0x5e3) & 3;
        if (uVar3 < 4) {
                    /* WARNING: Switch is manually overridden */
          switch((undefined4)*(undefined2 *)(uVar3 * 2 + 0x1a72)) {
          case 0x76f78:
            uVar8 = 0xf;
            pcVar7 = (char *)0x1160;
            break;
          case 0x76f8d:
            uVar8 = 0x1e;
            pcVar7 = (char *)0x1163;
            break;
          case 0x76f95:
            uVar8 = 0x3c;
            pcVar7 = (char *)0x1166;
            break;
          case 0x76f9d:
            sprintf((char *)CONCAT22(unaff_SS,local_1e),(char *)0x41251169);
            goto LAB_7563_197e;
          }
          sprintf((char *)CONCAT22(unaff_SS,local_1e),(char *)CONCAT22(0x4125,pcVar7),uVar8);
        }
LAB_7563_197e:
        FUN_7ecb_00d6(0x1b8,iVar4 * 0x1e + 0x86,local_1e);
        sprintf((char *)CONCAT22(unaff_SS,local_1e),(char *)0x4125116b,
                (int)*(char *)(local_6 * 0x16 + -0x7778));
        FUN_7ecb_00d6(0x1e8,iVar4 * 0x1e + 0x86,local_1e);
        sprintf((char *)CONCAT22(unaff_SS,local_1e),&LAB_4125_116c_2,
                (int)*(char *)(local_6 * 0x16 + -0x777a));
        FUN_7ecb_00d6(0x218,iVar4 * 0x1e + 0x86,local_1e);
        iVar4 = iVar4 + 1;
      }
    }
  } while( true );
}


