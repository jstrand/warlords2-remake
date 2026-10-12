
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_ui_build_production(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 unaff_SS;
  char *pcVar6;
  undefined2 uVar7;
  char local_5c [80];
  int local_c;
  int local_a;
  int local_8;
  
  FUN_22bf_07a5();
  if (param_1 == 0) {
    FUN_78a8_06ae(1,0xf,0,3);
    pcVar6 = get_string(0x70,0);
    FUN_7ecb_00d6(0x140,0x40,pcVar6);
    FUN_8611_0bf7(current_player,0,0x342a,0x4125,0x58,0x40);
    FUN_8611_0bf7(current_player,0,0x342a,0x4125,0x200,0x40);
    FUN_78a8_06ae(2,0xf,0,3);
    iVar4 = DAT_4125_0e98 * 0x41 + 0x1581;
    uVar7 = 0x3c04;
    iVar1 = current_player * 0x14;
    uVar5 = 0x3c04;
    pcVar6 = get_string(0x70,1);
    sprintf((char *)CONCAT22(unaff_SS,local_5c),pcVar6,iVar1,uVar5,iVar4,uVar7);
    FUN_7ecb_00d6(0x140,0x68,local_5c);
    FUN_54f6_070b(0xc4,0x131,0x98,0x14);
    FUN_78a8_06ae(2,0xf,0,3);
    uVar5 = *(undefined2 *)(current_player * 0x14 + 0x185);
    pcVar6 = get_string(0x70,2);
    sprintf((char *)CONCAT22(unaff_SS,local_5c),pcVar6,uVar5);
    FUN_7ecb_00d6(0x160,0x16d,local_5c);
    pcVar6 = get_string(0x70,3);
    FUN_7ecb_00d6(0xb0,0x15c,pcVar6);
  }
  for (iVar4 = 0; iVar4 < 4; iVar4 = iVar4 + 1) {
    if (*(char *)(DAT_4125_0e98 * 0x41 + iVar4 + 0x1593) < '\0') {
      iVar1 = 1;
      iVar2 = -1;
    }
    else {
      iVar1 = current_player + 2;
      iVar2 = (int)*(char *)(DAT_4125_0e98 * 0x41 + iVar4 + 0x1593);
    }
    FUN_8611_08be(iVar2,current_player,0x342a,0x4125,iVar4 * 0x28 + 0x68,0x172,0,0,0,0,iVar1,0,0);
    FUN_216d_002e(&local_a);
    if (_LAB_4125_0e86 == iVar4) {
      FUN_24d0_0002(0);
      FUN_216d_01fd(&local_a);
      local_a = local_a + -1;
      local_8 = local_8 + -1;
      FUN_24d0_0002(9);
    }
    else {
      FUN_24d0_0002(3);
      FUN_216d_01fd(&local_a);
      local_a = local_a + -1;
      local_8 = local_8 + -1;
    }
    FUN_216d_01fd(&local_a);
  }
  armytype_lock();
  FUN_78a8_06ae(2,0xf,0,3);
  iVar4 = 0;
  iVar1 = 0;
  do {
    if (0x1b < iVar4) {
      armytype_unlock();
      FUN_22bf_0757();
      return;
    }
    uVar5 = (undefined2)((ulong)armytype_table >> 0x10);
    iVar2 = (int)armytype_table;
    if (-1 < *(int *)(iVar2 + iVar4 * 0x3e + 0x1e)) {
      local_c = 0;
      for (iVar3 = 0; iVar3 < 4; iVar3 = iVar3 + 1) {
        if ((int)*(char *)(DAT_4125_0e98 * 0x41 + iVar3 + 0x1593) == *(int *)(iVar2 + iVar4 * 0x3e))
        {
          local_c = 1;
          break;
        }
      }
      if ((*(int *)(current_player * 0x14 + 0x185) < *(int *)(iVar2 + iVar4 * 0x3e + 0x1e)) ||
         (local_c != 0)) {
        uVar7 = 1;
      }
      else {
        uVar7 = 0;
      }
      FUN_8611_08be(*(undefined2 *)(iVar2 + iVar4 * 0x3e),current_player,0x342a,0x4125,
                    (iVar1 % 4) * 0x78 + 0x58,(iVar1 / 4) * 0x1f + 0x88,0,0,0,0,1,0,uVar7);
      if (param_1 == 0) {
        sprintf((char *)CONCAT22(unaff_SS,local_5c),(char *)0x41250e8c,
                *(undefined2 *)((int)armytype_table + iVar4 * 0x3e + 0x1e));
        FUN_7ecb_00bf((iVar1 % 4) * 0x78 + 0x80,(iVar1 / 4) * 0x1f + 0x8e,local_5c);
      }
      iVar1 = iVar1 + 1;
    }
    iVar4 = iVar4 + 1;
  } while( true );
}


