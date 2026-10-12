
void __cdecl16far FUN_7bab_1d0f(void)

{
  undefined2 uVar1;
  undefined2 in_DX;
  undefined4 uVar2;
  
  if (*(int *)(DAT_4125_2563 * 2 + 0xd0) == 1) {
    if (((*(int *)(DAT_4125_2563 * 2 + 0xc0) == 0) || (*(int *)(DAT_4125_2563 * 2 + 0xc0) == 1)) ||
       (*(int *)(DAT_4125_2563 * 2 + 0xc0) == 2)) {
      FUN_78a8_06ae(2);
    }
    FUN_54f6_070b(0xae0048,0x1400bc);
    FUN_7ecb_00bf(0xe000c0,(undefined1 *)&DAT_451b_08dc + DAT_451b_19f8 * 0x20,0x4125);
    FUN_54f6_070b(0xae0108,0x780070);
    uVar1 = FUN_7bab_20dc();
    FUN_7ecb_00bf(0xe00180,uVar1,in_DX);
    uVar1 = FUN_7bab_20dc();
    FUN_7ecb_00bf(0xf40180,uVar1,in_DX);
    uVar1 = FUN_7bab_20dc();
    FUN_7ecb_00bf(0x1080180,uVar1,in_DX);
    uVar1 = FUN_7bab_20dc();
    FUN_7ecb_00bf(0x11c0180,uVar1,in_DX);
    uVar1 = FUN_7bab_20dc();
    FUN_7ecb_00bf(0x1300180,uVar1,in_DX);
    uVar1 = FUN_7bab_20dc();
    uVar2 = CONCAT22(in_DX,uVar1);
    uVar1 = 0x144;
  }
  else {
    FUN_78a8_06ae(0xf0002,0x30000);
    FUN_7ecb_00bf(0xe000c4,0x260e,0x4125);
    uVar2 = 0x41252612;
    uVar1 = 0xe0;
  }
  FUN_7ecb_00bf(0x180,uVar1,uVar2);
  return;
}


