
void __cdecl16far FUN_5e97_053f(int param_1)

{
  byte bVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  undefined2 unaff_SS;
  undefined1 local_34 [15];
  byte abStack_25 [6];
  byte abStack_1f [6];
  byte abStack_19 [7];
  undefined2 local_12;
  int local_10;
  int local_e;
  int local_c;
  uint local_a;
  uint local_8;
  int local_6;
  uint local_4;
  
  local_8 = 0xffff;
  local_a = 0;
  local_c = 100;
  local_10 = 0;
  iVar4 = *(int *)(param_1 * 0x41 + 0x157d);
  local_12 = *(undefined2 *)(param_1 * 0x41 + 0x157f);
  uVar6 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  iVar5 = (int)DAT_451b_018e;
  if (*(char *)(iVar5 + param_1 + 0x56) != '\a') {
    iVar4 = iVar4 + 1;
  }
  if ((*(char *)(iVar5 + param_1 + 0x56) != '\a') || (0xb < *(byte *)(iVar5 + param_1 + 0x182))) {
    if ((*(int *)(current_player * 0x14 + 0x185) < 0x28) &&
       ((int)((undefined2 *)&side_income)[current_player] <
        (int)((undefined2 *)&side_upkeep)[current_player])) {
      local_e = 0x41;
    }
    else {
      local_e = 0x55;
    }
    local_6 = FUN_623c_12c9(iVar4,local_12,local_34);
    if (local_6 != 0) {
      if (local_6 < 4) {
        uVar6 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
        if (*(char *)((int)DAT_451b_018e + param_1 + 0x56) == '\x06') {
          return;
        }
        if (*(char *)((int)DAT_451b_018e + param_1 + 0x56) == '\a') {
          return;
        }
      }
      FUN_5db9_0656(param_1,0,abStack_19 + 1);
      iVar4 = 6;
      while (iVar5 = iVar4, iVar4 = iVar5 + -1, iVar5 != 0) {
        abStack_25[iVar5] = 0;
        local_4 = (uint)abStack_19[iVar5];
        if (local_4 != 0xff) {
          uVar6 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
          if ((((((*(byte *)((int)DAT_451b_018e + local_4 + 0x11e) & 1) == 0) &&
                (*(char *)(local_4 * 0x41 + 0x1592) != current_player)) &&
               (*(char *)(*(byte *)(*(int *)(local_4 * 0x41 + 0x157f) * 0xe0 +
                                   *(int *)(local_4 * 0x41 + 0x157d) * 2) + 0x710) == '\n')) &&
              (*(uint *)((int)DAT_451b_018e + 0x46) != local_4)) &&
             ((*(char *)(local_4 * 0x41 + 0x1592) == '\x0f' ||
              ((*(byte *)(current_player * 8 + (int)*(char *)(local_4 * 0x41 + 0x1592) + 0x153b) & 3
               ) == 2)))) {
            bVar1 = FUN_623c_15c5(*(undefined2 *)(local_4 * 0x41 + 0x157d),
                                  *(undefined2 *)(local_4 * 0x41 + 0x157f));
            abStack_25[iVar5] = bVar1;
            uVar2 = (uint)bVar1;
            if (uVar2 == 0) {
              if (((local_6 < 4) && (abStack_1f[iVar5] < 0x19)) ||
                 ((local_6 < 6 && (abStack_1f[iVar5] < 0xf)))) {
                local_10 = 1;
                abStack_25[iVar5] = 0xff;
              }
            }
            else if (local_e <= (int)uVar2) {
              if (DAT_451b_1ea0 + -2 < 1) {
                iVar3 = 1;
              }
              else {
                iVar3 = DAT_451b_1ea0 + -2;
              }
              iVar5 = (int)(uint)abStack_1f[iVar5] / iVar3 + 1;
              if ((iVar5 < local_c) || ((iVar5 == local_c && ((int)local_a < (int)uVar2)))) {
                local_8 = local_4;
                local_a = uVar2;
                local_c = iVar5;
              }
            }
          }
        }
      }
      if ((local_10 == 0) && (local_8 != 0xffff)) {
        FUN_623c_0ae7(local_34);
        uVar6 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
        move_stack_to(0x623c,*(undefined2 *)((int)DAT_451b_1f02 + 0x12),
                      *(undefined2 *)((int)DAT_451b_1f02 + 0x14),0);
      }
    }
  }
  return;
}


