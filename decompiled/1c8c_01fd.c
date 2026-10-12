
int __cdecl16far auto_tutorial_hook_2(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  undefined2 uVar4;
  char *pcVar5;
  int local_8;
  int local_6;
  int local_4;
  
  if ((((DAT_451b_1f02 == (int *)0x0) || (param_1 < 0)) || (param_2 < 0)) ||
     ((0x6f < param_1 || (0x9b < param_2)))) {
    local_4 = 0;
  }
  else {
    if ((param_1 == 0) && (param_2 == 0)) {
      FUN_22bf_0af4(&local_8);
      param_1 = DAT_3c04_0177 + (local_8 + -0x10) / 0x28;
      param_2 = DAT_3c04_0175 + (local_6 + -0x1e) / 0x28;
    }
    local_4 = move_stack_step(param_1,param_2,0);
    uVar4 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
    piVar3 = (int *)DAT_451b_1f02;
    if ((piVar3[9] == *DAT_451b_1f02) && (piVar3[1] == piVar3[10])) {
      piVar3[10] = -1;
      piVar3[9] = -1;
    }
    if (local_4 == 0) {
      FUN_8611_006c();
    }
    if (local_4 == 2) {
      FUN_8c07_08fb();
      FUN_1b62_08b3(1,0x10001);
      if ((opt_tutorial != 0) && (iVar1 = game_turn(), iVar1 == 1)) {
        pcVar5 = get_file_string(0x19,9);
        FUN_8065_168d((char *)pcVar5,(int)((ulong)pcVar5 >> 0x10));
      }
      local_4 = 2;
    }
    else {
      FUN_8065_0174();
      if (opt_tutorial != 0) {
        if ((DAT_3c04_0134 & 0x10) == 0) {
          for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
            local_6 = *DAT_451b_1f02 + *(int *)((int)(undefined2 *)&DAT_4125_2b4b + iVar1 * 2 + 1);
            local_8 = ((int *)DAT_451b_1f02)[1] + ((undefined2 *)&DAT_4125_2b5c)[iVar1];
            iVar2 = FUN_828e_044a(local_6,local_8);
            if ((-1 < iVar2) && (*(char *)(iVar2 * 0x41 + 0x1592) != current_player)) {
              DAT_3c04_0134 = DAT_3c04_0134 | 0x10;
              pcVar5 = get_file_string(0x19,4);
              FUN_8065_168d((char *)pcVar5,(int)((ulong)pcVar5 >> 0x10));
              break;
            }
          }
        }
        if (-1 < (char)DAT_3c04_0134) {
          uVar4 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
          if ((*(char *)(*(byte *)(((int *)DAT_451b_1f02)[1] * 0xe0 + *DAT_451b_1f02 * 2) + 0x710)
               == '\v') && ((char)((int *)DAT_451b_1f02)[2] == '\x1c')) {
            DAT_3c04_0134 = DAT_3c04_0134 | 0x80;
            pcVar5 = get_file_string(0x19,5);
            FUN_8065_168d((char *)pcVar5,(int)((ulong)pcVar5 >> 0x10));
          }
        }
      }
    }
  }
  return local_4;
}


