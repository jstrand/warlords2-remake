
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __cdecl16far auto_ui_armies_sunk(int param_1,int param_2)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 unaff_SS;
  char *fmt;
  undefined2 local_38 [8];
  int local_28 [16];
  int local_8;
  int local_6;
  int local_4;
  
  local_8 = 0;
  for (iVar3 = 0; iVar3 < 8; iVar3 = iVar3 + 1) {
    local_38[iVar3] = 1;
  }
  if ((((param_1 < 0) || (param_2 < 0)) ||
      (*(char *)(*(byte *)(param_2 * 0xe0 + param_1 * 2) + 0x710) == '\x02')) ||
     ((*(byte *)(param_2 * 0xe0 + param_1 * 2 + 1) & 0x80) != 0)) {
    if ((param_1 < 0) || (param_2 < 0)) {
      local_6 = army_count;
      while (local_6 = local_6 + -1, local_6 != 0) {
        iVar3 = *(int *)(local_6 * 0x16 + -0x7780);
        local_4 = *(int *)(local_6 * 0x16 + -0x777e);
        if (((-1 < iVar3) && (-1 < local_4)) &&
           ((*(char *)(local_6 * 0x16 + -0x777b) == current_player &&
            ((((iVar3 < 0 || (local_4 < 0)) ||
              (*(char *)(*(byte *)(local_4 * 0xe0 + iVar3 * 2) + 0x710) == '\x02')) ||
             ((*(byte *)(local_4 * 0xe0 + iVar3 * 2 + 1) & 0x80) != 0)))))) {
          iVar4 = 0;
          iVar2 = army_count;
          while (iVar2 = iVar2 + -1, iVar2 != 0) {
            if (((*(int *)(iVar2 * 0x16 + -0x7780) == iVar3) &&
                (*(int *)(iVar2 * 0x16 + -0x777e) == local_4)) &&
               (*(char *)(iVar2 * 0x16 + -0x777b) == current_player)) {
              local_28[iVar4 * 2 + 1] = 0x2e1d;
              local_28[iVar4 * 2] = iVar2 * 0x16 + -0x7780;
              iVar4 = iVar4 + 1;
            }
          }
          iVar3 = FUN_1c8c_07fa(local_28);
          if (iVar3 == 0) {
            for (iVar3 = 0; iVar3 < iVar4; iVar3 = iVar3 + 1) {
              uVar1 = (undefined2)((ulong)*(undefined4 *)(local_28 + iVar3 * 2) >> 0x10);
              if ((((*(uint *)((int)*(undefined4 *)(local_28 + iVar3 * 2) + 0xc) & 0x1000) == 0) &&
                  (*(char *)(local_28[iVar3 * 2] + 4) != '\x1c')) &&
                 (((undefined1 *)&army_move_flags)[*(char *)(local_28[iVar3 * 2] + 4) * 6] == '\0'))
              {
                local_8 = local_8 + 1;
                FUN_1b62_03be((int)*(undefined4 *)(local_28 + iVar3 * 2),
                              (int)((ulong)*(undefined4 *)(local_28 + iVar3 * 2) >> 0x10));
              }
            }
          }
        }
      }
    }
    else {
      iVar2 = 0;
      iVar3 = army_count;
      while (iVar4 = iVar3 + -1, iVar3 != 0) {
        iVar3 = iVar4;
        if (((*(int *)(iVar4 * 0x16 + -0x7780) == param_1) &&
            (*(int *)(iVar4 * 0x16 + -0x777e) == param_2)) &&
           (*(char *)(iVar4 * 0x16 + -0x777b) == current_player)) {
          local_28[iVar2 * 2 + 1] = 0x2e1d;
          local_28[iVar2 * 2] = iVar4 * 0x16 + -0x7780;
          iVar2 = iVar2 + 1;
        }
      }
      iVar3 = FUN_1c8c_07fa(local_28);
      if (iVar3 == 0) {
        for (iVar3 = 0; iVar3 < iVar2; iVar3 = iVar3 + 1) {
          uVar1 = (undefined2)((ulong)*(undefined4 *)(local_28 + iVar3 * 2) >> 0x10);
          if ((((*(uint *)((int)*(undefined4 *)(local_28 + iVar3 * 2) + 0xc) & 0x1000) == 0) &&
              (*(char *)(local_28[iVar3 * 2] + 4) != '\x1c')) &&
             (((undefined1 *)&army_move_flags)[*(char *)(local_28[iVar3 * 2] + 4) * 6] == '\0')) {
            local_8 = local_8 + 1;
            FUN_1b62_03be((int)*(undefined4 *)(local_28 + iVar3 * 2),
                          (int)((ulong)*(undefined4 *)(local_28 + iVar3 * 2) >> 0x10));
          }
        }
      }
    }
    if (0 < local_8) {
      sound_splash();
      _DAT_451b_1efe = 0;
      DAT_451b_1f02 = 0;
      for (iVar3 = 0; iVar3 < 8; iVar3 = iVar3 + 1) {
        *(undefined2 *)((undefined *)&DAT_451b_1ee0 + iVar3 * 4) = 0;
        ((undefined2 *)&DAT_451b_1ede)[iVar3 * 2] = 0;
      }
      DAT_4125_2bda = 0;
      DAT_451b_1e9e = 0;
      refresh_map_owners();
      FUN_8611_006c();
      if (local_8 == 1) {
        iVar3 = 2;
      }
      else {
        iVar3 = 0;
      }
      iVar2 = local_8;
      fmt = get_string(0xa8,iVar3);
      sprintf(&DAT_451b_1d28,fmt,iVar2);
      get_string(0xa8,1);
      FUN_8065_1160();
      if ((-1 < param_1) && (-1 < param_2)) {
        FUN_8611_0629(param_1,param_2);
      }
    }
    if (local_8 < 1) {
      uVar1 = 0;
    }
    else {
      uVar1 = 1;
    }
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}


