
void __cdecl16far FUN_2012_0043(int param_1)

{
  int iVar1;
  undefined2 uVar2;
  
  DAT_4125_34e7 = (uint *)CONCAT22(0x4125,(undefined2 *)&DAT_4125_3465 + param_1 * 0xd);
  FUN_2012_0005(((undefined2 *)&DAT_4125_3467)[param_1 * 0xd],
                ((undefined2 *)&DAT_4125_346b)[param_1 * 0xd],
                ((undefined2 *)&DAT_4125_3471)[param_1 * 0xd],
                ((undefined2 *)&DAT_4125_346f)[param_1 * 0xd]);
  FUN_2012_0187(((uint *)DAT_4125_34e7)[9]);
  FUN_2553_000d(((uint *)DAT_4125_34e7)[10]);
  uVar2 = (undefined2)((ulong)DAT_4125_34e7 >> 0x10);
  FUN_216d_002e(0x342a,0x4125,0,((uint *)DAT_4125_34e7)[2],((uint *)DAT_4125_34e7)[3]);
  uVar2 = (undefined2)((ulong)DAT_4125_34e7 >> 0x10);
  DAT_4125_3432 = ((uint *)DAT_4125_34e7)[1];
  DAT_4125_345c = 0;
  DAT_4125_345d = 0;
  DAT_4125_345e = (byte)((uint *)DAT_4125_34e7)[7];
  for (iVar1 = 0; iVar1 < (int)(uint)DAT_4125_345e; iVar1 = iVar1 + 1) {
    ((undefined4 *)&DAT_4125_3444)[iVar1] = (ulong)((uint *)DAT_4125_34e7)[4] << 0x10;
  }
  DAT_4125_3454 = 0;
  DAT_4125_345a = 0x4125;
  DAT_4125_3458 = (undefined2 *)&DAT_451b_2a78;
  DAT_4125_345f = DAT_4125_345f | 2;
  if (*DAT_4125_34e7 < 5) {
                    /* WARNING: Switch is manually overridden */
    switch((undefined4)*(undefined2 *)(*DAT_4125_34e7 * 2 + 0x17d)) {
    case 0x20236:
      DAT_451b_2a76 = 0x1f00;
      DAT_451b_2a74 = 0x3a1;
      DAT_451b_2a6a = 0x1f00;
      DAT_451b_2a68 = 0xc4d;
      DAT_451b_2a72 = 0x1f00;
      DAT_451b_2a70 = 0xc;
      DAT_451b_2a6e = 0x1f00;
      DAT_451b_2a6c = 0x19c;
      break;
    case 0x20268:
    }
  }
  DAT_451b_2a66 = 0x1e68;
  DAT_451b_2a64 = 0x24a;
  DAT_451b_2a62 = 0x1e68;
  DAT_451b_2a60 = 0x21a;
  DAT_451b_2a5e = 0x1e68;
  DAT_451b_2a5c = 0x1e8;
  DAT_451b_2a5a = 0x2012;
  DAT_451b_2a58 = 0x1199;
  return;
}


