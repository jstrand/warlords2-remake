
void __cdecl16far FUN_834b_12d3(long param_1,undefined4 param_2)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 unaff_SS;
  int local_10;
  int local_e;
  int local_c;
  int local_a;
  uint local_8;
  int local_6;
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
    for (local_4 = 0; local_4 < city_count; local_4 = local_4 + 1) {
      iVar1 = *(int *)(local_4 * 0x41 + 0x157d);
      iVar4 = *(int *)(local_4 * 0x41 + 0x157f);
      if (((opt_hidden_map == 0) || ((*(byte *)(iVar4 * 0x70 + iVar1) & 0x20) != 0)) ||
         (((*(byte *)(iVar4 * 0x70 + iVar1 + 1) & 0x20) != 0 ||
          (((*(byte *)(iVar4 * 0x70 + iVar1 + 0x70) & 0x20) != 0 ||
           ((*(byte *)(iVar4 * 0x70 + iVar1 + 0x71) & 0x20) != 0)))))) {
        uVar2 = iVar1 * 2 - 1;
        iVar4 = iVar4 * 2;
        local_8 = uVar2 & 7;
        iVar3 = ((int)uVar2 >> 3) * 8;
        uVar5 = (undefined2)((ulong)param_2 >> 0x10);
        iVar1 = (int)param_2;
        if (*(char *)(iVar1 + local_4) == -1) {
          local_6 = -1;
        }
        else if (*(char *)(iVar1 + local_4) == '\x0f') {
          local_6 = 8;
        }
        else {
          local_6 = (int)*(char *)(iVar1 + local_4);
        }
        if (local_6 < 0) {
          FUN_1997_034a();
          FUN_1997_060a();
          (*DAT_451b_2a68)(0x1997,0x57a7,0x4125,DAT_4125_2c6e,
                           DAT_4125_2c70 + local_8 * DAT_4125_2c72._2_2_,DAT_4125_2c72,0x6795,0x4125
                           ,iVar3,iVar4 + -1,0x576c,0x4125,DAT_4125_2c6e,
                           DAT_4125_2c70 + local_8 * DAT_4125_2c72._2_2_);
          FUN_1997_06bb();
          FUN_24d0_0002();
          FUN_2133_00b8(0x6795,0x4125,iVar3 + local_8 + 1,iVar4,iVar3 + local_8 + 6,iVar4 + 5,0);
          FUN_2133_00b8(0x6795,0x4125,iVar3 + local_8 + 1,iVar4 + 6,iVar3 + local_8 + 6,iVar4,0);
        }
        else {
          FUN_1997_034a();
          FUN_1997_060a();
          (*DAT_451b_2a68)(0x1997,0x57a7,0x4125,*(undefined2 *)(local_6 * 8 + 0x2c2e),
                           *(int *)(local_6 * 8 + 0x2c30) + local_8 * *(int *)(local_6 * 8 + 0x2c34)
                           ,*(undefined2 *)(local_6 * 8 + 0x2c32),
                           *(undefined2 *)(local_6 * 8 + 0x2c34),0x6795,0x4125,iVar3,iVar4 + -1,
                           0x576c,0x4125,*(undefined2 *)(local_6 * 8 + 0x2c2e),
                           *(int *)(local_6 * 8 + 0x2c30) + local_8 * *(int *)(local_6 * 8 + 0x2c34)
                          );
          FUN_1997_06bb();
        }
      }
    }
  }
  return;
}


