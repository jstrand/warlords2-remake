
undefined2 __cdecl16far auto_tutorial_hook_5(void)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 in_DX;
  int iVar3;
  char *pcVar4;
  undefined4 local_6;
  
  FUN_828e_0af3();
  FUN_177b_003d();
  uVar1 = FUN_8c07_040e();
  local_6 = CONCAT22(in_DX,uVar1);
  if (local_6 == 0) {
    DAT_451b_1f02 = (int *)0x0;
    sound_chord();
  }
  else {
    FUN_8c07_06eb(local_6);
    sound_ding();
  }
  FUN_89e0_0356();
  FUN_8065_0174();
  if (opt_tutorial != 0) {
    if ((DAT_3c04_0134 & 8) == 0) {
      DAT_3c04_0134 = DAT_3c04_0134 | 8;
      pcVar4 = get_file_string(0x19,3);
      FUN_8065_168d((char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
    }
    if ((DAT_3c04_0134 & 0x10) == 0) {
      for (iVar3 = 0; iVar3 < 8; iVar3 = iVar3 + 1) {
        iVar2 = FUN_828e_044a(*DAT_451b_1f02 +
                              *(int *)((int)(undefined2 *)&DAT_4125_2b4b + iVar3 * 2 + 1),
                              ((int *)DAT_451b_1f02)[1] + ((undefined2 *)&DAT_4125_2b5c)[iVar3]);
        if ((-1 < iVar2) && (*(char *)(iVar2 * 0x41 + 0x1592) != current_player)) {
          DAT_3c04_0134 = DAT_3c04_0134 | 0x10;
          pcVar4 = get_file_string(0x19,4);
          FUN_8065_168d((char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
          break;
        }
      }
    }
    if (-1 < (char)DAT_3c04_0134) {
      uVar1 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
      if ((*(char *)(*(byte *)(((int *)DAT_451b_1f02)[1] * 0xe0 + *DAT_451b_1f02 * 2) + 0x710) ==
           '\v') && ((char)((int *)DAT_451b_1f02)[2] == '\x1c')) {
        DAT_3c04_0134 = DAT_3c04_0134 | 0x80;
        pcVar4 = get_file_string(0x19,5);
        FUN_8065_168d((char *)pcVar4,(int)((ulong)pcVar4 >> 0x10));
      }
    }
  }
  return 1;
}


