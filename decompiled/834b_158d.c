
void __cdecl16far FUN_834b_158d(long param_1,int param_2,int param_3,int param_4,int param_5)

{
  byte *pbVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  undefined2 unaff_SS;
  int local_10;
  int local_e;
  int local_c;
  int local_a;
  int local_8;
  uint local_6;
  int local_4;
  
  if (((opt_hidden_map == 0) || (*(int *)(current_player * 2 + 0xd0) == 0)) || (DAT_3c04_0159 != 0))
  {
    if (param_1 == 0) {
      param_1._0_2_ = 0x2a88;
      param_1._2_2_ = 0x4125;
    }
    else {
    }
    __F_SCOPY((undefined2)param_1,param_1._2_2_,&local_10,unaff_SS);
    local_10 = local_10 + -8;
    local_c = local_c + 0xc;
    local_e = local_e + -8;
    local_a = local_a + 0xc;
    for (iVar2 = 0; iVar2 < 0x70; iVar2 = iVar2 + 1) {
      for (iVar4 = 0; iVar4 < 0x9c; iVar4 = iVar4 + 1) {
        pbVar1 = (byte *)(iVar4 * 0xe0 + iVar2 * 2 + 1);
        *pbVar1 = *pbVar1 & 0xef;
      }
    }
    for (local_8 = 0; local_8 < army_count; local_8 = local_8 + 1) {
      iVar2 = *(int *)(local_8 * 0x16 + -0x7780);
      iVar4 = *(int *)(local_8 * 0x16 + -0x777e);
      if ((((-1 < iVar2) && (-1 < iVar4)) &&
          (((*(byte *)(iVar4 * 0xe0 + iVar2 * 2 + 1) & 0x10) == 0 &&
           ((pbVar1 = (byte *)(iVar4 * 0xe0 + iVar2 * 2 + 1), *pbVar1 = *pbVar1 | 0x10,
            opt_hidden_map == 0 || ((*(byte *)(iVar4 * 0x70 + iVar2) & 0x20) != 0)))))) &&
         ((param_5 == 0 || (*(char *)(*(byte *)(iVar4 * 0xe0 + iVar2 * 2) + 0x710) != '\n')))) {
        local_4 = (int)*(char *)(local_8 * 0x16 + -0x777b);
        if (local_4 == 0xf) {
          local_4 = 8;
        }
        if (((param_4 == 0) || (current_player != local_4)) &&
           (((param_2 < 0 || (*(char *)(local_8 * 0x16 + -0x777b) == param_2)) &&
            ((param_3 < 0 || (*(char *)(local_8 * 0x16 + -0x777c) == param_3)))))) {
          uVar3 = iVar2 * 2 - 1;
          iVar4 = iVar4 * 2 + -1;
          local_6 = uVar3 & 7;
          iVar2 = ((int)uVar3 >> 3) << 3;
          if (iVar2 < 0) {
            iVar2 = 0;
          }
          if (iVar4 < 0) {
            iVar4 = 0;
          }
          FUN_1997_034a(9);
          FUN_1997_060a(9);
          (*DAT_451b_2a68)(0x1997,0x57a7,0x4125,*(undefined2 *)(local_4 * 4 + 0x2cbe),
                           *(int *)(local_4 * 4 + 0x2cc0) + local_6 * DAT_4125_2ce6._2_2_,
                           DAT_4125_2ce6,0x6795,0x4125,iVar2,iVar4,0x576c,0x4125,
                           *(undefined2 *)(local_4 * 4 + 0x2cbe),
                           *(int *)(local_4 * 4 + 0x2cc0) + local_6 * DAT_4125_2ce6._2_2_);
          FUN_1997_06bb(9);
        }
      }
    }
  }
  return;
}


