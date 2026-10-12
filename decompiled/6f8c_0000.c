
int __cdecl16far city_production_turn(void)

{
  byte *pbVar1;
  undefined1 *puVar2;
  char *pcVar3;
  char cVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  undefined2 uVar9;
  undefined2 uVar10;
  undefined2 unaff_SS;
  int local_18;
  undefined4 local_16;
  int local_12;
  int local_10;
  int local_e;
  int local_c;
  int local_a;
  undefined2 local_8;
  undefined2 local_6;
  int local_4;
  
  func_0x0006002d();
  uVar10 = 0x7563;
  FUN_7563_0bd9();
  *(undefined1 *)((int)DAT_451b_0504 + 800) = 0;
  iVar6 = army_count;
LAB_6f8c_04b1:
  while( true ) {
    while( true ) {
      while( true ) {
        do {
          iVar7 = iVar6 + -1;
          if (iVar6 == 0) {
            local_6 = 1;
            for (local_4 = 0; local_4 < city_count; local_4 = local_4 + 1) {
              if ((*(char *)(local_4 * 0x41 + 0x1592) == current_player) &&
                 ('\0' < *(char *)(local_4 * 0x41 + 0x15aa))) {
                *(char *)(local_4 * 0x41 + 0x15aa) =
                     *(char *)(local_4 * 0x41 + 0x15aa) - (char)local_6;
                if (((*(char *)(local_4 * 0x41 + 0x15aa) < '\x01') &&
                    (*(undefined1 *)(local_4 * 0x41 + 0x15aa) = 0,
                    0 < *(int *)(current_player * 0x14 + 0x185))) &&
                   ((iVar6 = func_0x000604bf(uVar10,local_4,0,&local_8), iVar6 != 0 ||
                    (*(int *)((undefined1 *)&LAB_4125_15ad_1 + local_4 * 0x41) != 0)))) {
                  if (*(int *)((undefined1 *)&LAB_4125_15ad_1 + local_4 * 0x41) == 0) {
                    local_c = -1;
                    uVar8 = (undefined2)((ulong)DAT_451b_0504 >> 0x10);
                    *(undefined1 *)((int)DAT_451b_0504 + (int)*(char *)((int)DAT_451b_0504 + 800)) =
                         2;
                  }
                  else {
                    uVar10 = 0x828e;
                    local_c = FUN_828e_044a(*(undefined2 *)
                                             ((undefined1 *)&LAB_4125_15b0 + local_4 * 0x41));
                    uVar8 = (undefined2)((ulong)DAT_451b_0504 >> 0x10);
                    *(undefined1 *)((int)DAT_451b_0504 + (int)*(char *)((int)DAT_451b_0504 + 800)) =
                         1;
                    if (local_c < 0) {
                      local_c = current_player + 100;
                    }
                  }
                  local_16 = func_0x00060697(uVar10,local_4,local_8);
                  uVar8 = (undefined2)((ulong)local_16 >> 0x10);
                  uVar9 = (undefined2)((ulong)DAT_451b_0504 >> 0x10);
                  iVar6 = (int)DAT_451b_0504 + *(char *)((int)DAT_451b_0504 + 800) * 4;
                  *(undefined2 *)(iVar6 + 0x192) = uVar8;
                  *(int *)(iVar6 + 400) = (int)local_16;
                  *(undefined1 *)((int)DAT_451b_0504 + *(char *)((int)DAT_451b_0504 + 800) + 100) =
                       *(undefined1 *)((int)local_16 + 4);
                  uVar8 = (undefined2)((ulong)DAT_451b_0504 >> 0x10);
                  *(undefined1 *)
                   ((int)DAT_451b_0504 + (int)*(char *)((int)DAT_451b_0504 + 800) + 200) =
                       (undefined1)local_4;
                  *(undefined1 *)((int)DAT_451b_0504 + *(char *)((int)DAT_451b_0504 + 800) + 300) =
                       (undefined1)local_c;
                  *(char *)((int)DAT_451b_0504 + 800) = *(char *)((int)DAT_451b_0504 + 800) + '\x01'
                  ;
                }
              }
            }
            cVar4 = *(char *)((int)DAT_451b_0504 + 800);
            func_0x00060093();
            return (int)cVar4;
          }
          iVar6 = iVar7;
        } while (*(char *)(iVar7 * 0x16 + -0x777b) != current_player);
        if (((*(int *)(iVar7 * 0x16 + -0x7780) != -1) || (*(int *)(iVar7 * 0x16 + -0x777e) != -1))
           || (*(char *)(iVar7 * 0x16 + -0x7770) != 'e')) break;
        *(undefined1 *)(iVar7 * 0x16 + -0x7770) = 0x66;
      }
      if (((*(int *)(iVar7 * 0x16 + -0x7780) != -2) || (*(int *)(iVar7 * 0x16 + -0x777e) != -1)) ||
         (*(char *)(iVar7 * 0x16 + -0x7770) != 'e')) break;
      *(undefined1 *)(iVar7 * 0x16 + -0x7770) = 0x66;
    }
    if (((*(int *)(iVar7 * 0x16 + -0x7780) != -1) || (*(int *)(iVar7 * 0x16 + -0x777e) != -1)) ||
       (*(char *)(iVar7 * 0x16 + -0x7770) != 'f')) goto LAB_6f8c_02b5;
    iVar5 = func_0x000604bf(uVar10,*(byte *)(iVar7 * 0x16 + -0x7771) >> 1,0,&local_8);
    if ((iVar5 != 0) && (iVar5 = FUN_5f19_147f((int)*(char *)(iVar7 * 0x16 + -0x777c)), iVar5 != 0))
    break;
    if ((int)*(char *)(iVar7 * 0x16 + -0x7777) == (uint)(*(byte *)(iVar7 * 0x16 + -0x7771) >> 1)) {
      uVar10 = 0x1b62;
      FUN_1b62_03be(iVar7 * 0x16 + -0x7780);
    }
    else {
      cVar4 = *(char *)(iVar7 * 0x16 + -0x7777);
      pbVar1 = (byte *)(iVar7 * 0x16 + -0x7771);
      *pbVar1 = *pbVar1 & 1;
      pbVar1 = (byte *)(iVar7 * 0x16 + -0x7771);
      *pbVar1 = *pbVar1 | cVar4 << 1;
      *(undefined1 *)(iVar7 * 0x16 + -0x7770) = 0x65;
    }
  }
  uVar8 = (undefined2)((ulong)DAT_451b_0504 >> 0x10);
  *(undefined1 *)((int)DAT_451b_0504 + (int)*(char *)((int)DAT_451b_0504 + 800)) = 0;
  iVar5 = (int)DAT_451b_0504 + *(char *)((int)DAT_451b_0504 + 800) * 4;
  *(undefined2 *)(iVar5 + 0x192) = 0x2e1d;
  *(int *)(iVar5 + 400) = iVar7 * 0x16 + -0x7780;
  *(undefined1 *)((int)DAT_451b_0504 + *(char *)((int)DAT_451b_0504 + 800) + 100) =
       *(undefined1 *)(iVar7 * 0x16 + -0x777c);
  uVar8 = (undefined2)((ulong)DAT_451b_0504 >> 0x10);
  *(undefined1 *)((int)DAT_451b_0504 + (int)*(char *)((int)DAT_451b_0504 + 800) + 200) =
       *(undefined1 *)(iVar7 * 0x16 + -0x7777);
  uVar8 = (undefined2)((ulong)DAT_451b_0504 >> 0x10);
  *(byte *)((int)DAT_451b_0504 + (int)*(char *)((int)DAT_451b_0504 + 800) + 300) =
       *(byte *)(iVar7 * 0x16 + -0x7771) >> 1;
  *(undefined2 *)(iVar7 * 0x16 + -0x7780) = local_8;
  iVar5 = local_a;
  goto LAB_6f8c_046e;
LAB_6f8c_02b5:
  if (((*(int *)(iVar7 * 0x16 + -0x7780) == -2) && (*(int *)(iVar7 * 0x16 + -0x777e) == -1)) &&
     (*(char *)(iVar7 * 0x16 + -0x7770) == 'f')) {
    local_e = FUN_7563_0b47(current_player,&local_10);
    uVar10 = 0x1b62;
    armies_at_tile(local_10,local_12,(int *)CONCAT22(unaff_SS,&local_18));
    if (((local_e == 0) || (7 < local_18)) ||
       (iVar5 = FUN_5f19_147f((int)*(char *)(iVar7 * 0x16 + -0x777c)), iVar5 == 0)) {
      *(undefined2 *)(iVar7 * 0x16 + -0x7780) = 0xffff;
      cVar4 = *(char *)(iVar7 * 0x16 + -0x7777);
      pbVar1 = (byte *)(iVar7 * 0x16 + -0x7771);
      *pbVar1 = *pbVar1 & 1;
      pbVar1 = (byte *)(iVar7 * 0x16 + -0x7771);
      *pbVar1 = *pbVar1 | cVar4 << 1;
      *(undefined1 *)(iVar7 * 0x16 + -0x7770) = 0x65;
    }
    else {
      uVar8 = (undefined2)((ulong)DAT_451b_0504 >> 0x10);
      *(undefined1 *)((int)DAT_451b_0504 + (int)*(char *)((int)DAT_451b_0504 + 800)) = 0;
      iVar5 = (int)DAT_451b_0504 + *(char *)((int)DAT_451b_0504 + 800) * 4;
      *(undefined2 *)(iVar5 + 0x192) = 0x2e1d;
      *(int *)(iVar5 + 400) = iVar7 * 0x16 + -0x7780;
      *(undefined1 *)((int)DAT_451b_0504 + *(char *)((int)DAT_451b_0504 + 800) + 100) =
           *(undefined1 *)(iVar7 * 0x16 + -0x777c);
      uVar8 = (undefined2)((ulong)DAT_451b_0504 >> 0x10);
      *(undefined1 *)((int)DAT_451b_0504 + (int)*(char *)((int)DAT_451b_0504 + 800) + 200) =
           *(undefined1 *)(iVar7 * 0x16 + -0x7777);
      uVar8 = (undefined2)((ulong)DAT_451b_0504 >> 0x10);
      *(byte *)((int)DAT_451b_0504 + (int)*(char *)((int)DAT_451b_0504 + 800) + 300) =
           *(byte *)(iVar7 * 0x16 + -0x7771) >> 1;
      *(int *)(iVar7 * 0x16 + -0x7780) = local_10;
      iVar5 = local_12;
LAB_6f8c_046e:
      *(int *)(iVar7 * 0x16 + -0x777e) = iVar5;
      pbVar1 = (byte *)(iVar7 * 0x16 + -0x7771);
      *pbVar1 = *pbVar1 & 1;
      pbVar1 = (byte *)(iVar7 * 0x16 + -0x7772);
      *pbVar1 = *pbVar1 & 0xf0;
      puVar2 = (undefined1 *)(iVar7 * 0x16 + -0x7772);
      *puVar2 = *puVar2;
      *(undefined1 *)(iVar7 * 0x16 + -0x7770) = 0xff;
      pcVar3 = (char *)((int)DAT_451b_0504 + 800);
      *pcVar3 = *pcVar3 + '\x01';
    }
  }
  goto LAB_6f8c_04b1;
}


