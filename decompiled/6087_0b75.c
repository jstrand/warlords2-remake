
undefined2 __cdecl16far FUN_6087_0b75(int param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_SS;
  int local_20 [8];
  int local_10;
  int local_e;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  uint local_4;
  
  uVar3 = (undefined2)((ulong)param_2 >> 0x10);
  iVar2 = (int)param_2;
  iVar1 = *(int *)(iVar2 + param_1 * 2);
  local_e = *(int *)(iVar2 + 0x38);
  local_8 = *(int *)(local_e * 0x41 + 0x157d);
  local_a = *(int *)(local_e * 0x41 + 0x157f);
  local_6 = *(int *)(iVar2 + param_1 * 2 + 0xc);
  local_c = map_distance(local_8,local_a,*(int *)(iVar1 * 0x16 + -0x7780),
                         *(int *)(iVar1 * 0x16 + -0x777e));
  iVar2 = ai_collect_stack(*(int *)(iVar1 * 0x16 + -0x7780),*(int *)(iVar1 * 0x16 + -0x777e),
                           (int *)CONCAT22(unaff_SS,local_20),
                           (*(byte *)(iVar1 * 0x16 + -0x7772) & 0x70) >> 4,
                           *(byte *)(iVar1 * 0x16 + -0x7772) & 0xf,0xc);
  if (iVar2 != 0) {
    if ((local_6 != -1) && (*(byte *)((int)DAT_451b_018e + local_6 + 0x182) == local_4)) {
      local_10 = FUN_57ea_0a3e(*(undefined2 *)(iVar1 * 0x16 + -0x7780),
                               *(undefined2 *)(iVar1 * 0x16 + -0x777e),0xffff);
      iVar1 = game_turn();
      if (iVar1 < 6) {
        iVar1 = 10;
      }
      else {
        iVar1 = 0xf;
      }
      if (local_10 < iVar1) {
        return 0;
      }
    }
    FUN_623c_0ae7(local_20);
    move_stack_to();
  }
  return 0;
}


