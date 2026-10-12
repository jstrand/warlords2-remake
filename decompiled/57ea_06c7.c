
void __cdecl16far
FUN_57ea_06c7(uint param_1,char param_2,undefined4 param_3,undefined4 param_4,undefined2 *param_5)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 unaff_DI;
  uint uVar6;
  undefined2 uVar7;
  char *unaff_SS;
  undefined1 local_18 [5];
  byte abStack_13 [7];
  uint local_c;
  int local_a;
  int local_8;
  uint local_6;
  int local_4;
  
  local_a = 100;
  local_c = param_1;
  while( true ) {
    iVar1 = 8;
    while( true ) {
      iVar5 = iVar1 + -1;
      iVar4 = (int)param_4;
      uVar7 = (undefined2)((ulong)param_4 >> 0x10);
      if (iVar1 == 0) break;
      iVar1 = iVar5;
      if ((*(int *)(iVar4 + iVar5 * 2) != -1) &&
         (*(char *)(*(int *)(iVar4 + iVar5 * 2) * 0x16 + -0x777b) != current_player)) {
        *(undefined2 *)(iVar4 + iVar5 * 2) = 0xffff;
      }
    }
    if (opt_neutral_cities != 0) {
      FUN_623c_14d3(iVar4,uVar7);
    }
    local_4 = 1000;
    iVar1 = ai_neutral_neighbours
                      (local_c,(char *)CONCAT22(local_18,unaff_SS),
                       (char *)CONCAT22(unaff_DI,unaff_SS));
    if (iVar1 == 0) {
      return;
    }
    iVar1 = 6;
    uVar6 = 0xffff;
    while( true ) {
      iVar4 = iVar1;
      iVar1 = iVar4 + -1;
      uVar7 = (undefined2)((ulong)param_3 >> 0x10);
      if (iVar4 == 0) break;
      local_6 = (uint)abStack_13[iVar4];
      if (((local_6 != 0xff) && ((*(byte *)((int)DAT_451b_018e + local_6 + 0x11e) & 1) == 0)) &&
         (*param_5 = 0, *(char *)((int)param_3 + local_6) < '\x03')) {
        if (opt_neutral_cities != 0) {
          local_a = FUN_623c_15c5(*(undefined2 *)(local_6 * 0x41 + 0x157d),
                                  *(undefined2 *)(local_6 * 0x41 + 0x157f));
        }
        if (0x4a < local_a) {
          iVar5 = dice(1,10,0);
          if ((byte)(&stack0xffe7)[iVar4] < 0x29) {
            iVar2 = 0;
          }
          else {
            iVar2 = 10;
          }
          if ((byte)(&stack0xffe7)[iVar4] < 0x33) {
            iVar3 = 0;
          }
          else {
            iVar3 = 0x1e;
          }
          iVar4 = iVar5 + (uint)(byte)(&stack0xffe7)[iVar4] + *(char *)((int)param_3 + local_6) * 10
                  + iVar2 + iVar3 + (100 - local_a);
          if (iVar4 < local_4) {
            local_4 = iVar4;
            local_8 = local_a;
            uVar6 = local_6;
          }
        }
      }
    }
    if (uVar6 == 0xffff) break;
    *(char *)((int)param_3 + uVar6) = *(char *)((int)param_3 + uVar6) + param_2;
    DAT_451b_0160 = param_1;
    DAT_4125_07c6 = uVar6;
    FUN_623c_0ae7(param_4,1,uVar6,0x80);
    move_stack_to();
    if (*(char *)(uVar6 * 0x41 + 0x1592) != current_player) {
      return;
    }
    if (*(int *)((int)DAT_451b_018e + 0x48) != 0) {
      return;
    }
    local_c = uVar6;
    iVar1 = FUN_57ea_0a3e();
    if (iVar1 < 10) {
      return;
    }
  }
  if (((opt_hidden_map != 0) && (DAT_451b_0160 == param_1)) && (DAT_4125_07c6 != 0xffff)) {
    FUN_623c_0ae7(param_4,1,DAT_4125_07c6,0x80);
  }
  return;
}


