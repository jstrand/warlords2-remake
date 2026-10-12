
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far
ui_army_info(int param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4,undefined2 param_5
            ,int param_6)

{
  int iVar1;
  undefined2 in_DX;
  undefined2 extraout_DX;
  undefined2 unaff_SS;
  undefined2 uVar2;
  char local_3e [50];
  int local_c;
  undefined4 local_a [2];
  
  if (param_1 != -1) {
    FUN_740d_1486(local_a);
    iVar1 = FUN_2012_01fe(0x4494,0x4125,(int)local_a[1],(int)((ulong)local_a[1] >> 0x10));
    if (iVar1 != 0) {
      FUN_22bf_07a5();
      if ((DAT_451b_053e != 0) && ((DAT_451b_0569 & 10) == 0)) {
        for (local_c = 0; local_c < (int)(uint)DAT_451b_0568; local_c = local_c + 1) {
          uVar2 = mem_lock((int)((undefined4 *)&DAT_451b_053e)[local_c],
                           (int)((ulong)((undefined4 *)&DAT_451b_053e)[local_c] >> 0x10));
          ((undefined2 *)&DAT_451b_0550)[local_c * 2] = in_DX;
          ((undefined2 *)&DAT_451b_054e)[local_c * 2] = uVar2;
        }
      }
      (*DAT_451b_2a74)();
      if ((DAT_451b_053e != 0) && ((DAT_451b_0569 & 10) == 0)) {
        for (local_c = 0; local_c < (int)(uint)DAT_451b_0568; local_c = local_c + 1) {
          mem_unlock((int)((undefined4 *)&DAT_451b_053e)[local_c],
                     (int)((ulong)((undefined4 *)&DAT_451b_053e)[local_c] >> 0x10));
        }
      }
      _DAT_451b_0530 = local_a[0];
      __F_SCOPY(local_a,unaff_SS,(undefined2 *)&DAT_451b_0528,0x4125);
      FUN_1997_034a(0x28);
      (*DAT_451b_2a68)(0x1997,0x54a8,0x4125,_LAB_4125_1050,_LAB_4125_1054,0x342a,0x4125,
                       (int)_DAT_451b_0530,(int)((ulong)_DAT_451b_0530 >> 0x10),0x546d,0x4125,
                       (int)_LAB_4125_1050,(int)((ulong)_LAB_4125_1050 >> 0x10));
      uVar2 = extraout_DX;
      FUN_78a8_06ae(2,0xf,0,3);
      if (param_1 == 0x1c) {
        iVar1 = param_6 * 0x14 + 0x223;
        uVar2 = 0x3c04;
      }
      else {
        iVar1 = FUN_7087_14f4(param_1);
      }
      FUN_7ecb_00d6(DAT_451b_0530 + 0x70,DAT_451b_0532 + 8,iVar1,uVar2);
      FUN_8611_08be(param_1,current_player,0x342a,0x4125,DAT_451b_0530 + 0x60,DAT_451b_0532 + 0x1f,0
                    ,0,0,0,1,0,0);
      if ((param_1 != 0x1c) && (0 < param_6)) {
        (*DAT_451b_2a74)();
        if (1 < param_6) {
          (*DAT_451b_2a74)();
        }
        if (2 < param_6) {
          (*DAT_451b_2a74)();
        }
        if (3 < param_6) {
          (*DAT_451b_2a74)();
        }
      }
      FUN_78a8_06ae(2,0xf,0,3);
      sprintf((char *)CONCAT22(unaff_SS,local_3e),(char *)0x412510a8,param_3);
      FUN_7ecb_0103(DAT_451b_0530 + 0x68,DAT_451b_0532 + 0x42,local_3e);
      sprintf((char *)CONCAT22(unaff_SS,local_3e),(char *)0x412510b5,param_2);
      FUN_7ecb_00bf(DAT_451b_0530 + 0x78,DAT_451b_0532 + 0x42,local_3e);
      sprintf((char *)CONCAT22(unaff_SS,local_3e),&LAB_4125_10c0_2,param_5);
      FUN_7ecb_0103(DAT_451b_0530 + 0x68,DAT_451b_0532 + 0x56,local_3e);
      sprintf((char *)CONCAT22(unaff_SS,local_3e),(char *)0x412510cd,param_4);
      FUN_7ecb_00bf(DAT_451b_0530 + 0x78,DAT_451b_0532 + 0x56,local_3e);
      if (((((undefined1 *)&army_move_flags)[param_1 * 6] != '\0') ||
          (((((undefined1 *)&DAT_451b_0665)[param_1 * 6] != '\0' &&
            (((undefined1 *)&DAT_451b_0666)[param_1 * 6] != '\0')) ||
           (((undefined1 *)&DAT_451b_0665)[param_1 * 6] != '\0')))) ||
         (((undefined1 *)&DAT_451b_0666)[param_1 * 6] != '\0')) {
        (*DAT_451b_2a74)();
      }
    }
  }
  return;
}


