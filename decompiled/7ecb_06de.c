
void __cdecl16far auto_ui_quit_game(char *param_1,undefined2 param_2,undefined2 param_3)

{
  bool bVar1;
  int fd;
  char *pcVar2;
  void *pvVar3;
  uint unaff_DI;
  long lVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined1 auStack_6e [80];
  undefined2 uStack_1e;
  undefined2 uStack_1c;
  char local_19;
  undefined4 uStack_18;
  char *local_c;
  undefined4 local_8;
  undefined2 local_4;
  
  bVar1 = false;
  DAT_451b_1cca = param_2;
  DAT_451b_1cc8 = param_3;
  fd = open(param_1,2);
  if (fd < 0) {
    return;
  }
  lVar4 = filelength(fd);
  pvVar3 = (void *)((ulong)lVar4 >> 0x10);
  local_4 = (undefined2)lVar4;
  pcVar2 = (char *)FUN_1d28_01d5(local_4);
  local_8 = (char *)CONCAT22(pvVar3,pcVar2);
  local_c = local_8;
  read(fd,(void *)CONCAT22(local_4,pvVar3),unaff_DI);
  FUN_22bf_07a5();
override_jmp_7ecb_076d_case_1:
  do {
    if (bVar1) {
      FUN_1d28_050d((char *)local_8,(int)((ulong)local_8 >> 0x10));
      close(fd);
      FUN_22bf_0757();
      return;
    }
    while( true ) {
      pcVar2 = (char *)local_c;
      if (*local_c == '#') break;
      local_c = (char *)CONCAT22(local_c._2_2_,(char *)local_c + 1);
    }
    local_c = (char *)CONCAT22(local_c._2_2_,(char *)local_c + 1);
    local_19 = *local_c;
    local_c = (char *)CONCAT22(local_c._2_2_,pcVar2 + 2);
  } while (0x11 < (int)local_19 - 0x43U);
                    /* WARNING: Switch is manually overridden */
  switch((undefined4)*(undefined2 *)(((int)local_19 - 0x43U) * 2 + 0x8c5)) {
  case 0x7f422:
    FUN_7ecb_08e9(&local_c);
    uVar6 = uStack_1e;
    break;
  case 0x7f447:
    uVar5 = 1;
    uVar6 = 0xf;
    goto LAB_7ecb_078f;
  case 0x7f455:
    uVar6 = 0xf;
    break;
  case 0x7f45f:
    FUN_7ecb_0995(&local_c);
    FUN_7ecb_09e0(&local_c);
    FUN_7ecb_00d6((int)uStack_18,(int)((ulong)uStack_18 >> 0x10),auStack_6e);
    goto override_jmp_7ecb_076d_case_1;
  case 0x7f492:
    FUN_7ecb_0995(&local_c);
    FUN_7ecb_09e0(&local_c);
    FUN_7ecb_00bf((int)uStack_18,(int)((ulong)uStack_18 >> 0x10),auStack_6e);
    goto override_jmp_7ecb_076d_case_1;
  case 0x7f4c5:
    FUN_7ecb_0995(&local_c);
    FUN_7ecb_09e0(&local_c);
    FUN_7ecb_0103((int)uStack_18,(int)((ulong)uStack_18 >> 0x10),auStack_6e);
    goto override_jmp_7ecb_076d_case_1;
  case 0x7f4fb:
    FUN_7ecb_0924(&local_c);
    FUN_7ecb_08e9(&local_c);
    FUN_7ecb_0995(&local_c);
    __F_SPUSH((int)uStack_18,(int)((ulong)uStack_18 >> 0x10));
    FUN_1997_027e(uStack_1c);
    goto override_jmp_7ecb_076d_case_1;
  case 0x7f54f:
    bVar1 = true;
  case 0x7f552:
    goto override_jmp_7ecb_076d_case_1;
  }
  uVar5 = 2;
LAB_7ecb_078f:
  FUN_78a8_06ae(uVar5,uVar6,0,3);
  goto override_jmp_7ecb_076d_case_1;
}


