
void __cdecl16far FUN_796c_00f3(void)

{
  int iVar1;
  undefined2 uVar2;
  
  FUN_22bf_07a5();
  FUN_24d0_0002();
  FUN_216d_0288(0x15a6,0x4125);
  FUN_24d0_02e5(0x15a6,0x4125,0x20004);
  for (iVar1 = 0; iVar1 < 5; iVar1 = iVar1 + 1) {
    if (-1 < (int)((undefined2 *)&DAT_451b_08d0)[iVar1]) {
      if (iVar1 == DAT_451b_08da) {
        uVar2 = 0xf;
      }
      else {
        uVar2 = 2;
      }
      FUN_78a8_06ae(2,uVar2,0x30000);
      FUN_7ecb_00bf(*(undefined2 *)((undefined1 *)&LAB_4125_15ad_1 + iVar1 * 4),
                    *(undefined2 *)((undefined1 *)&LAB_4125_15b0 + iVar1 * 4),
                    (undefined1 *)&DAT_451b_08dc + ((undefined2 *)&DAT_451b_08d0)[iVar1] * 0x20,
                    0x4125);
    }
  }
  FUN_22bf_0757();
  return;
}


