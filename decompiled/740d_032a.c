
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far
ui_army_type_info(int param_1,undefined2 param_2,undefined2 param_3,undefined2 param_4,
                 undefined2 param_5)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 in_DX;
  undefined2 extraout_DX;
  undefined2 unaff_SS;
  char local_3c [50];
  undefined4 local_a [2];
  
  FUN_740d_1486(local_a);
  iVar1 = FUN_2012_01fe(0x4494,0x4125,(int)local_a[1],(int)((ulong)local_a[1] >> 0x10));
  if (iVar1 != 0) {
    FUN_22bf_07a5();
    if ((DAT_451b_053e != 0) && ((DAT_451b_0569 & 10) == 0)) {
      for (iVar1 = 0; iVar1 < (int)(uint)DAT_451b_0568; iVar1 = iVar1 + 1) {
        uVar2 = mem_lock((int)((undefined4 *)&DAT_451b_053e)[iVar1],
                         (int)((ulong)((undefined4 *)&DAT_451b_053e)[iVar1] >> 0x10));
        ((undefined2 *)&DAT_451b_0550)[iVar1 * 2] = in_DX;
        ((undefined2 *)&DAT_451b_054e)[iVar1 * 2] = uVar2;
      }
    }
    (*DAT_451b_2a74)();
    if ((DAT_451b_053e != 0) && ((DAT_451b_0569 & 10) == 0)) {
      for (iVar1 = 0; iVar1 < (int)(uint)DAT_451b_0568; iVar1 = iVar1 + 1) {
        mem_unlock((int)((undefined4 *)&DAT_451b_053e)[iVar1],
                   (int)((ulong)((undefined4 *)&DAT_451b_053e)[iVar1] >> 0x10));
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
    uVar3 = FUN_7087_14f4(param_1);
    FUN_7ecb_00d6(DAT_451b_0530 + 0x70,DAT_451b_0532 + 8,uVar3,uVar2);
    FUN_8611_08be(param_1,current_player,0x342a,0x4125,DAT_451b_0530 + 0x60,DAT_451b_0532 + 0x1f,0,0
                  ,0,0,1,0,0);
    sprintf((char *)CONCAT22(unaff_SS,local_3c),(char *)0x4125107c,param_3);
    FUN_7ecb_0103(DAT_451b_0530 + 0x68,DAT_451b_0532 + 0x42,local_3c);
    sprintf((char *)CONCAT22(unaff_SS,local_3c),(char *)0x41251089,param_5);
    FUN_7ecb_00bf(DAT_451b_0530 + 0x78,DAT_451b_0532 + 0x42,local_3c);
    sprintf((char *)CONCAT22(unaff_SS,local_3c),(char *)0x41251096,param_2);
    FUN_7ecb_0103(DAT_451b_0530 + 0x68,DAT_451b_0532 + 0x56,local_3c);
    sprintf((char *)CONCAT22(unaff_SS,local_3c),(char *)0x4125109f,param_4);
    FUN_7ecb_00bf(DAT_451b_0530 + 0x78,DAT_451b_0532 + 0x56,local_3c);
    if (((((undefined1 *)&army_move_flags)[param_1 * 6] != '\0') ||
        (((((undefined1 *)&DAT_451b_0665)[param_1 * 6] != '\0' &&
          (((undefined1 *)&DAT_451b_0666)[param_1 * 6] != '\0')) ||
         (((undefined1 *)&DAT_451b_0665)[param_1 * 6] != '\0')))) ||
       (((undefined1 *)&DAT_451b_0666)[param_1 * 6] != '\0')) {
      (*DAT_451b_2a74)();
    }
  }
  return;
}


