
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_tutorial_hook(void)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 unaff_SS;
  char *pcVar6;
  int local_8;
  int local_6;
  int local_4;
  
  bVar1 = false;
  _LAB_4125_1178 = 0;
  DAT_4125_2bdc = DAT_4125_2be0;
  iVar2 = FUN_7ecb_0a7c();
  if (iVar2 == 0) {
    _bios_timeofday();
  }
  FUN_22bf_0af4(&local_8);
  iVar2 = DAT_3c04_0177 + (local_8 + -0x10) / 0x28;
  iVar4 = DAT_3c04_0175 + (local_6 + -0x1e) / 0x28;
  if (DAT_451b_1f02 != (int *)0x0) {
    if ((*DAT_451b_1f02 == iVar2) && (((int *)DAT_451b_1f02)[1] == iVar4)) goto LAB_1b62_048d;
  }
  bVar1 = true;
LAB_1b62_048d:
  if (((bVar1) || (iVar3 = FUN_18a9_01cd(DAT_4125_2bdc,DAT_4125_2be0), iVar3 == 0)) ||
     (_LAB_4125_1172 != 0)) {
    armies_at_tile(iVar2,iVar4,(int *)CONCAT22(unaff_SS,&local_4));
    FUN_1b62_09ec(local_4,0);
    DAT_451b_1f02 = _DAT_451b_1ebe;
    FUN_8c07_06eb(_DAT_451b_1ebe);
    FUN_89e0_0356();
    FUN_8065_0174();
    if (opt_tutorial != 0) {
      if ((DAT_3c04_0134 & 8) == 0) {
        DAT_3c04_0134 = DAT_3c04_0134 | 8;
        pcVar6 = get_file_string(0x19,3);
        FUN_8065_168d((char *)pcVar6,(int)((ulong)pcVar6 >> 0x10));
      }
      if ((DAT_3c04_0134 & 0x10) == 0) {
        for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
          iVar4 = FUN_828e_044a(*DAT_451b_1f02 +
                                *(int *)((int)(undefined2 *)&DAT_4125_2b4b + iVar2 * 2 + 1),
                                ((int *)DAT_451b_1f02)[1] + ((undefined2 *)&DAT_4125_2b5c)[iVar2]);
          if ((-1 < iVar4) && (*(char *)(iVar4 * 0x41 + 0x1592) != current_player)) {
            DAT_3c04_0134 = DAT_3c04_0134 | 0x10;
            pcVar6 = get_file_string(0x19,4);
            FUN_8065_168d((char *)pcVar6,(int)((ulong)pcVar6 >> 0x10));
            break;
          }
        }
      }
      if ((DAT_3c04_0134 & 0x80) == 0) {
        uVar5 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
        if ((*(char *)(*(byte *)(((int *)DAT_451b_1f02)[1] * 0xe0 + *DAT_451b_1f02 * 2) + 0x710) ==
             '\v') && ((char)((int *)DAT_451b_1f02)[2] == '\x1c')) {
          DAT_3c04_0134 = DAT_3c04_0134 | 0x80;
          pcVar6 = get_file_string(0x19,5);
          FUN_8065_168d((char *)pcVar6,(int)((ulong)pcVar6 >> 0x10));
        }
      }
    }
  }
  else {
    FUN_89e0_0a55();
  }
  _LAB_4125_1172 = 1;
  return;
}


