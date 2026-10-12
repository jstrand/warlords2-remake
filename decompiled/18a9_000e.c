
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_18a9_000e(void)

{
  bool bVar1;
  int iVar2;
  uint local_18;
  undefined2 uStack_16;
  long local_8;
  int iStack_4;
  
  local_8 = 0;
  FUN_18a9_01e7(500,0x18a9);
  FUN_18a9_025f();
  FUN_18a9_01f9();
override_jmp_18a9_007c_case_5:
  do {
    do {
      bVar1 = 30000 < local_8;
      local_8 = local_8 + 1;
      if (bVar1) {
        local_8 = 0;
        FUN_7ecb_0a1b();
      }
      if (DAT_4125_166c != 0) {
        FUN_18a9_02f1();
      }
      FUN_251d_008a();
      FUN_21b5_0003(0xffffffff,&local_18);
    } while (7 < local_18);
                    /* WARNING: Switch is manually overridden */
    switch((undefined4)*(undefined2 *)(local_18 * 2 + 0x11c)) {
    case 0x18ac4:
      goto override_jmp_18a9_007c_case_5;
    case 0x18b11:
      sound_poll();
      if ((((DAT_4125_166a == 0) && (_LAB_4125_1176 == 0)) && (_LAB_4125_1172 == 0)) &&
         (_LAB_4125_1174 == 0)) {
        FUN_177b_011f();
        FUN_2372_11fd();
        (*DAT_451b_0cd8)(0x2372);
      }
      iStack_4 = FUN_18a9_0756();
      if (iStack_4 != 0) {
        DAT_451b_0cde = iStack_4;
      }
      iVar2 = FUN_1726_0009(0);
      if (iVar2 == 0) goto override_jmp_18a9_007c_case_5;
      iVar2 = FUN_17be_0064(iVar2,0);
      break;
    case 0x18b74:
      FUN_18a9_012c(&local_18);
      goto override_jmp_18a9_007c_case_5;
    case 0x18b80:
      DAT_4125_1670 = 1;
      FUN_18a9_0189(&local_18);
      goto override_jmp_18a9_007c_case_5;
    case 0x18b96:
      iVar2 = FUN_17be_0064(uStack_16,1);
    }
    if (iVar2 != 0) {
      return;
    }
  } while( true );
}


