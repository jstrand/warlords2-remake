
void __cdecl16far FUN_89e0_0e85(int param_1,int param_2,int param_3)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_SS;
  char local_18 [10];
  int local_e;
  int iStack_c;
  uint local_a;
  int local_8;
  int local_6;
  int local_4;
  
  FUN_22bf_07a5();
  if (param_1 != 0) {
    FUN_54f6_0000(0);
    __F_SPUSH((int)DAT_4125_3102,(int)((ulong)DAT_4125_3102 >> 0x10));
    FUN_1997_027e(0x2e);
    FUN_78a8_06ae(2,0xf);
    sprintf(&DAT_451b_1d28,s__max___d__4125_320d);
    FUN_7ecb_00bf(0x1e0,0x42);
  }
  FUN_89e0_1b9c();
  local_4 = param_3;
  if (param_2 == 0) {
    local_4 = 0;
    param_3 = 7;
  }
  local_6 = param_3;
  FUN_78a8_06ae(2,0xf);
  for (iVar1 = 0; iVar2 = local_4, iVar1 < DAT_4125_30f6; iVar1 = iVar1 + 1) {
    if (((undefined2 *)&DAT_451b_28d2)[iVar1] == -1) {
      if (param_1 == 0) {
        FUN_54f6_070b(0,iVar1 * 0x1e + 0x23);
      }
    }
    else {
      _local_e = CONCAT22(((undefined2 *)&DAT_4125_3108)[iVar1 * 2] + 5,
                          ((undefined2 *)&DAT_4125_3106)[iVar1 * 2] + -0x20);
      __F_SPUSH();
      FUN_1997_0129(8);
    }
  }
  for (; iVar2 <= local_6; iVar2 = iVar2 + 1) {
    if (iVar2 < DAT_4125_30f6) {
      local_8 = (current_player + ((undefined2 *)&DAT_451b_28f2)[iVar2]) % 8 + 2;
      local_a = (uint)(((undefined2 *)&DAT_451b_28e2)[iVar2] == 0);
      FUN_8611_08be((int)*(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_2902 + iVar2 * 2) +
                                  4),current_player,0x342a,0x4125,
                    ((undefined2 *)&DAT_4125_3106)[iVar2 * 2],
                    ((undefined2 *)&DAT_4125_3108)[iVar2 * 2],0,0,0,0,local_8);
      if (param_1 == 0) {
        FUN_54f6_070b(0x40,iVar2 * 0x1e + 0x23);
      }
      uVar3 = (undefined2)((ulong)*(undefined4 *)((undefined2 *)&DAT_451b_2902 + iVar2 * 2) >> 0x10)
      ;
      if ((*(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_2902 + iVar2 * 2) + 4) != '\x1c')
         && (((undefined1 *)&DAT_451b_0668)
             [*(char *)(((undefined2 *)&DAT_451b_2902)[iVar2 * 2] + 4) * 6] == '\0')) {
        for (iVar1 = 0;
            iVar1 < *(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_2902 + iVar2 * 2) + 10);
            iVar1 = iVar1 + 1) {
          FUN_216d_000e(&local_e);
          __F_SPUSH();
          FUN_1997_0129(8);
        }
      }
      if (*(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_2902 + iVar2 * 2) + 4) != '\x1c') {
        FUN_7087_14f4();
      }
      FUN_7ecb_00bf(((undefined2 *)&DAT_4125_3106)[iVar2 * 2] + 0x30,
                    ((undefined2 *)&DAT_4125_3108)[iVar2 * 2] + 5);
      if (*(char *)(((undefined2 *)&DAT_451b_2902)[iVar2 * 2] + 4) == '\x1c') {
        hero_battle_items();
      }
      sprintf((char *)CONCAT22(unaff_SS,local_18),(char *)0x41253217);
      FUN_7ecb_00bf(((undefined2 *)&DAT_4125_3106)[iVar2 * 2] + 0xa8,
                    ((undefined2 *)&DAT_4125_3108)[iVar2 * 2] + 5);
      if (((undefined2 *)&DAT_451b_28e2)[iVar2] != 0) {
        sprintf((char *)CONCAT22(unaff_SS,local_18),(char *)0x4125321a);
        FUN_7ecb_00bf(((undefined2 *)&DAT_4125_3106)[iVar2 * 2] + 0xb8,
                      ((undefined2 *)&DAT_4125_3108)[iVar2 * 2] + 5);
      }
      sprintf((char *)CONCAT22(unaff_SS,local_18),(char *)0x4125321f);
      FUN_7ecb_00bf(((undefined2 *)&DAT_4125_3106)[iVar2 * 2] + 0xe8,
                    ((undefined2 *)&DAT_4125_3108)[iVar2 * 2] + 5);
      FUN_216d_000e(&local_e);
      uVar3 = (undefined2)((ulong)*(undefined4 *)((undefined2 *)&DAT_451b_2902 + iVar2 * 2) >> 0x10)
      ;
      if ((((((undefined1 *)&army_move_flags)
             [*(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_2902 + iVar2 * 2) + 4) * 6] !=
             '\0') ||
           (((*(char *)(((undefined2 *)&DAT_451b_2902)[iVar2 * 2] + 4) == '\x1c' &&
             (iVar1 = hero_flight_items(), iVar1 != 0)) ||
            ((*(uint *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_2902 + iVar2 * 2) + 0xc) &
             0x1000) != 0)))) ||
          (((uVar3 = (undefined2)
                     ((ulong)*(undefined4 *)((undefined2 *)&DAT_451b_2902 + iVar2 * 2) >> 0x10),
            ((undefined1 *)&DAT_451b_0665)
            [*(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_2902 + iVar2 * 2) + 4) * 6] !=
            '\0' && (((undefined1 *)&DAT_451b_0666)
                     [*(char *)(((undefined2 *)&DAT_451b_2902)[iVar2 * 2] + 4) * 6] != '\0')) ||
           (((undefined1 *)&DAT_451b_0665)
            [*(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_2902 + iVar2 * 2) + 4) * 6] !=
            '\0')))) ||
         (((undefined1 *)&DAT_451b_0666)
          [*(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_2902 + iVar2 * 2) + 4) * 6] !=
          '\0')) {
        __F_SPUSH();
        FUN_1997_0129(8);
      }
      auto_ui_army_bonus_text
                ((int)*(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_2902 + iVar2 * 2) + 4))
      ;
      FUN_7ecb_00bf(((undefined2 *)&DAT_4125_3106)[iVar2 * 2] + 0x130,
                    ((undefined2 *)&DAT_4125_3108)[iVar2 * 2] + 5);
    }
    else {
      FUN_8611_08be(0xffff,current_player,0x342a,0x4125,((undefined2 *)&DAT_4125_3106)[iVar2 * 2],
                    ((undefined2 *)&DAT_4125_3108)[iVar2 * 2],0,0,0,0,1);
    }
  }
  FUN_22bf_0757();
  return;
}


