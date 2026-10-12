
undefined2 __cdecl16far move_stack_step(undefined2 param_1,undefined2 param_2,int param_3)

{
  int iVar1;
  undefined2 *puVar2;
  undefined2 uVar3;
  undefined1 local_da [16];
  undefined1 local_ca [200];
  
  uVar3 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
  puVar2 = (undefined2 *)DAT_451b_1f02;
  puVar2[9] = param_1;
  puVar2[10] = param_2;
  FUN_1555_1868(local_da);
  iVar1 = pathfind(local_da);
  if (iVar1 == 0) {
    if (*(int *)(current_player * 2 + 0xd0) == 0) {
      sound_chord();
    }
    return 1;
  }
  if (param_3 == 0) {
    FUN_1c8c_0963(*DAT_451b_1f02,((undefined2 *)DAT_451b_1f02)[1],local_ca);
  }
  else {
    DAT_4125_2eaa = 0;
    DAT_4125_2ea6 = 0;
    DAT_4125_2ea8 = 0;
  }
  uVar3 = walk_path(local_da);
  if (DAT_451b_1e64 != 0) {
    sea_change_spend_moves();
    uVar3 = 2;
  }
  if (param_3 == 0) {
    FUN_89e0_070d();
    FUN_8961_07f0();
    FUN_89e0_0356();
  }
  return uVar3;
}


