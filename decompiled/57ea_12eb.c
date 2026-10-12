
undefined2 __cdecl16far FUN_57ea_12eb(int param_1,int param_2,int param_3)

{
  uint *puVar1;
  byte *pbVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  undefined2 unaff_SS;
  int local_16 [8];
  int local_6;
  int local_4;
  
  local_6 = FUN_6087_056d(param_2,param_3,&local_4);
  if ((((local_6 != -1) && ((*(uint *)(param_1 * 0x16 + -0x7774) & 0x1000) == 0)) &&
      ((local_4 != 1 ||
       (uVar3 = FUN_6087_063c(), (*(uint *)(param_1 * 0x16 + -0x7774) & uVar3) == 0)))) &&
     (((*(char *)(param_1 * 0x16 + -0x777c) == '\x1c' || (local_4 == 1)) &&
      ((uVar3 = *(byte *)(param_3 * 0xe0 + param_2 * 2 + 1) & 0xf, uVar3 == 0xf ||
       (current_player == uVar3)))))) {
    iVar5 = 8;
    while (iVar5 != 0) {
      local_16[iVar5 + -1] = -1;
      iVar5 = iVar5 + -1;
    }
    local_16[0] = param_1;
    FUN_623c_0ae7(local_16);
    uVar6 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
    iVar5 = (int)DAT_451b_1f02;
    *(int *)(iVar5 + 0x12) = param_2;
    *(int *)(iVar5 + 0x14) = param_3;
    iVar5 = move_stack_to(0x623c,*(undefined2 *)(iVar5 + 0x12),*(undefined2 *)(iVar5 + 0x14),0);
    if (iVar5 != 1) {
      if ((*(int *)(param_1 * 0x16 + -0x7780) == param_2) &&
         (*(int *)(param_1 * 0x16 + -0x777e) == param_3)) {
        DAT_451b_1f02 = CONCAT22(0x2e1d,param_1 * 0x16 + -0x7780);
        if (*(char *)(param_1 * 0x16 + -0x777c) == '\x1c') {
          FUN_5ad0_15c3(param_1,local_6);
        }
        else {
          site_search();
        }
        iVar5 = army_count;
        if (local_4 == 5) {
          while (iVar4 = iVar5 + -1, iVar5 != 0) {
            iVar5 = iVar4;
            if ((((*(char *)(iVar4 * 0x16 + -0x777b) == (char)current_player) &&
                 ((*(byte *)(iVar4 * 0x16 + -0x7772) & 0xf) == 0)) &&
                (((undefined1 *)&DAT_451b_0668)[*(char *)(iVar4 * 0x16 + -0x777c) * 6] != '\0')) &&
               ((*(int *)(iVar4 * 0x16 + -0x7780) == param_2 &&
                (*(int *)(iVar4 * 0x16 + -0x777e) == param_3)))) {
              puVar1 = (uint *)(iVar4 * 0x16 + -0x7774);
              *puVar1 = *puVar1 | 0x20;
            }
          }
        }
      }
      else {
        if ((local_4 == 1) && ((*(uint *)(param_1 * 0x16 + -0x7774) & 0x1000) != 0)) {
          uVar3 = FUN_6087_063c(local_6);
          puVar1 = (uint *)(param_1 * 0x16 + -0x7774);
          *puVar1 = *puVar1 | uVar3;
        }
        puVar1 = (uint *)(param_1 * 0x16 + -0x7774);
        *puVar1 = *puVar1 | 0x40;
      }
      puVar1 = (uint *)(param_1 * 0x16 + -0x7774);
      *puVar1 = *puVar1 | 0x20;
      pbVar2 = (byte *)(param_1 * 0x16 + -0x7772);
      *pbVar2 = *pbVar2 & 0xf0;
      pbVar2 = (byte *)(param_1 * 0x16 + -0x7771);
      *pbVar2 = *pbVar2 & 1;
      *(undefined2 *)(param_1 * 0x16 + -0x776c) = 0xffff;
      *(undefined2 *)(param_1 * 0x16 + -0x776e) = 0xffff;
      return 1;
    }
  }
  return 0;
}


