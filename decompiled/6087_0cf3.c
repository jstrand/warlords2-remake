
undefined2 __cdecl16far auto_str_quest_city_going_dist(int param_1,undefined4 param_2)

{
  int iVar1;
  uint uVar2;
  int y1;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  int local_a;
  
  iVar1 = *(int *)((int)param_2 + param_1 * 2);
  if ((*(int *)(current_player * 0xc + 0x1105) == 4) ||
     (*(int *)(current_player * 0xc + 0x1105) == 5)) {
    uVar2 = *(uint *)(current_player * 0xc + 0x1109);
    iVar5 = *(int *)(uVar2 * 0x41 + 0x157d);
    y1 = *(int *)(uVar2 * 0x41 + 0x157f);
    if ((*(byte *)((int)DAT_451b_018e + uVar2 + 0x11e) & 1) == 0) {
      if ((*(char *)(uVar2 * 0x41 + 0x1592) == '\x0f') || (iVar3 = game_turn(), iVar3 < 8)) {
        iVar3 = 1;
      }
      else {
        iVar3 = 2;
      }
      iVar4 = FUN_623c_12c9();
      if (iVar3 <= iVar4) {
        if (((*(byte *)(iVar1 * 0x16 + -0x7772) & 0xf) != 1) ||
           (*(byte *)(iVar1 * 0x16 + -0x7771) >> 1 != uVar2)) {
          local_a = map_distance(iVar5,y1,*(int *)(iVar1 * 0x16 + -0x7780),
                                 *(int *)(iVar1 * 0x16 + -0x777e));
          iVar5 = FUN_623c_15c5(*(undefined2 *)(uVar2 * 0x41 + 0x157d),
                                *(undefined2 *)(uVar2 * 0x41 + 0x157f));
          if (iVar5 < 0x4b) {
            return 0;
          }
        }
        sprintf(&DAT_451b_1d28,s_Quest_city__d_GOING____dist__d__s_4125_0993,uVar2 + 1,local_a,
                DAT_451b_1e9c,DAT_451b_1e9a);
        FUN_623c_170c((undefined1 *)&DAT_451b_1d28,0x4125,*(undefined2 *)(iVar1 * 0x16 + -0x7780),
                      *(undefined2 *)(iVar1 * 0x16 + -0x777e));
        FUN_623c_0ae7();
        uVar6 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
        move_stack_to(0x623c,*(undefined2 *)((int)DAT_451b_1f02 + 0x12),
                      *(undefined2 *)((int)DAT_451b_1f02 + 0x14),0);
        return 1;
      }
    }
  }
  return 0;
}


