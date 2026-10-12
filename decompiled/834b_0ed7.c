
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_834b_0ed7(long param_1,int param_2)

{
  bool bVar1;
  undefined2 uVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  undefined2 unaff_SS;
  undefined4 local_16;
  int local_10;
  int local_e;
  int local_c;
  int local_a;
  uint local_8;
  int local_6;
  int local_4;
  
  if ((((opt_hidden_map == 0) || (*(int *)(current_player * 2 + 0xd0) == 0)) || (DAT_3c04_0159 != 0)
      ) || (DAT_4125_2d28 != 0)) {
    if (((param_1 == 0) || (8 < *(int *)((int)param_1 + 4))) || (8 < *(int *)((int)param_1 + 6))) {
      bVar1 = false;
    }
    else {
      bVar1 = true;
    }
    if (bVar1) {
      uVar2 = 0x2cea;
    }
    else {
      uVar2 = 0x6795;
    }
    local_16 = CONCAT22(0x4125,uVar2);
    iVar5 = (int)param_1;
    uVar2 = param_1._2_2_;
    if (param_1 == 0) {
      iVar5 = 0x2a88;
      uVar2 = 0x4125;
    }
    __F_SCOPY(iVar5,uVar2,&local_10,unaff_SS);
    local_10 = local_10 + -8;
    local_c = local_c + 0xc;
    local_e = local_e + -8;
    local_a = local_a + 0xc;
    for (local_4 = 0; local_4 < city_count; local_4 = local_4 + 1) {
      iVar5 = *(int *)(local_4 * 0x41 + 0x157d);
      iVar3 = *(int *)(local_4 * 0x41 + 0x157f);
      if (opt_hidden_map == 0) goto LAB_834b_10ee;
      if ((param_1 == 0) ||
         (((local_10 <= iVar5 && (iVar5 <= local_10 + local_c + -1)) &&
          ((local_e <= iVar3 && (iVar3 <= local_e + local_a + -1)))))) {
        if (DAT_4125_3254 == 0) {
          if ((((*(byte *)(iVar3 * 0x70 + iVar5) & 0x20) != 0) ||
              ((*(byte *)(iVar3 * 0x70 + iVar5 + 1) & 0x20) != 0)) ||
             (((*(byte *)(iVar3 * 0x70 + iVar5 + 0x70) & 0x20) != 0 ||
              ((*(byte *)(iVar3 * 0x70 + iVar5 + 0x71) & 0x20) != 0)))) {
LAB_834b_10ee:
            uVar4 = iVar5 * 2 - 1;
            local_6 = iVar3 * 2 + -1;
            local_8 = uVar4 & 7;
            iVar5 = ((int)uVar4 >> 3) * 8;
            if (bVar1) {
              iVar5 = iVar5 - DAT_451b_1f3a;
              local_6 = local_6 - _DAT_451b_1f3c;
              if (((iVar5 < 0) || (local_6 < 0)) || ((0x10 < iVar5 || (0x16 < local_6))))
              goto LAB_834b_12bb;
            }
            if (*(char *)(*(byte *)(*(int *)(local_4 * 0x41 + 0x157f) * 0xe0 +
                                   *(int *)(local_4 * 0x41 + 0x157d) * 2) + 0x710) == '\n') {
              if (*(char *)(local_4 * 0x41 + 0x1592) == '\x0f') {
                iVar3 = 8;
              }
              else {
                iVar3 = (int)*(char *)(local_4 * 0x41 + 0x1592);
              }
            }
            else {
              iVar3 = -1;
            }
            if (-1 < iVar3) {
              FUN_1997_034a();
              FUN_1997_060a();
              (*DAT_451b_2a68)(0x1997,0x57a7,0x4125,*(undefined2 *)(iVar3 * 8 + 0x2c2e),
                               *(int *)(iVar3 * 8 + 0x2c30) + local_8 * *(int *)(iVar3 * 8 + 0x2c34)
                               ,*(undefined2 *)(iVar3 * 8 + 0x2c32),
                               *(undefined2 *)(iVar3 * 8 + 0x2c34),local_16,iVar5,local_6,0x576c,
                               0x4125,*(undefined2 *)(iVar3 * 8 + 0x2c2e),
                               *(int *)(iVar3 * 8 + 0x2c30) + local_8 * *(int *)(iVar3 * 8 + 0x2c34)
                              );
              FUN_1997_06bb();
            }
            if (local_4 == param_2) {
              iVar5 = iVar5 + local_8;
              FUN_2012_0bb7(0x6795,0x4125,iVar5 + -1,local_6 + -1,0xf000a);
              FUN_2012_0bb7(0x6795,0x4125,iVar5 + -1,local_6 + 9,0xf000a);
              FUN_2012_0f08(0x6795,0x4125,iVar5 + -1,local_6 + -1,0xf000a);
              FUN_2012_0f08(0x6795,0x4125,iVar5 + 9,local_6 + -1,0xf000a);
            }
          }
        }
        else if ((((*(char *)(iVar3 * 0x70 + iVar5) < '\0') ||
                  (*(char *)(iVar3 * 0x70 + iVar5 + 1) < '\0')) ||
                 (*(char *)(iVar3 * 0x70 + iVar5 + 0x70) < '\0')) ||
                (*(char *)(iVar3 * 0x70 + iVar5 + 0x71) < '\0')) goto LAB_834b_10ee;
      }
LAB_834b_12bb:
    }
  }
  return;
}


