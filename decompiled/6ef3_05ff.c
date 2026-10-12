
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_6ef3_05ff(void)

{
  bool bVar1;
  undefined2 uVar2;
  undefined4 in_EDX;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 unaff_SS;
  char local_20 [10];
  undefined1 local_16 [8];
  int local_e;
  undefined2 local_c;
  int local_a;
  undefined2 local_8;
  int local_6;
  int local_4;
  
  if (DAT_4125_0df6 == 3) {
    FUN_6ef3_093d();
  }
  else {
    uVar5 = 0x1997;
    FUN_1997_034a(5);
    if ((DAT_451b_1679 != 0) && ((DAT_451b_16a4 & 10) == 0)) {
      for (iVar3 = 0; iVar3 < (int)(uint)DAT_451b_16a3; iVar3 = iVar3 + 1) {
        uVar5 = 0x1d28;
        uVar2 = mem_lock((int)((undefined4 *)&DAT_451b_1679)[iVar3],
                         (int)((ulong)((undefined4 *)&DAT_451b_1679)[iVar3] >> 0x10));
        ((undefined2 *)&DAT_451b_168b)[iVar3 * 2] = (int)in_EDX;
        ((undefined2 *)&DAT_451b_1689)[iVar3 * 2] = uVar2;
      }
    }
    for (iVar3 = 0; iVar3 < 8; iVar3 = iVar3 + 1) {
      if (((undefined2 *)&DAT_451b_04e0)[iVar3] == 0) {
        local_c = *(undefined2 *)(iVar3 * 2 + 0xe10);
        local_8 = 8;
        local_e = 0x138;
        if (_LAB_4125_0e3f_1 < 1) {
          local_a = 1;
        }
        else {
          if ((int)((undefined2 *)&DAT_451b_04f0)[iVar3] < 0) {
            ((undefined2 *)&DAT_451b_04f0)[iVar3] = 0;
          }
          local_a = (int)(((long)(int)((undefined2 *)&DAT_451b_04f0)[iVar3] * 0xf0) /
                         (long)_LAB_4125_0e3f_1);
        }
        if (local_a < 1) {
          local_a = 1;
        }
        iVar4 = 0;
        local_4 = local_a / 8;
        local_6 = local_a % 8;
        bVar1 = false;
        while (!bVar1) {
          if (iVar4 == local_4) {
            bVar1 = true;
            if (0 < local_6) {
              (*DAT_451b_2a74)(uVar5,0x55cf,0x4125,*(undefined2 *)(iVar3 * 4 + 0xe20),
                               *(undefined2 *)(iVar3 * 4 + 0xe22),local_6,8,0x342a,0x4125,
                               local_e + iVar4 * 8,local_c);
            }
          }
          else {
            (*DAT_451b_2a74)(uVar5,0x55cf,0x4125,*(undefined2 *)(iVar3 * 4 + 0xe20),
                             *(undefined2 *)(iVar3 * 4 + 0xe22),8,8,0x342a,0x4125,
                             local_e + iVar4 * 8,local_c);
            iVar4 = iVar4 + 1;
          }
          FUN_216d_002e(local_16);
          uVar5 = 0x24d0;
          FUN_24d0_02e5(local_16);
        }
      }
    }
    FUN_24d0_0002(1);
    FUN_2133_02fe(0x138,0xc4,0xf0);
    FUN_2133_0344(0x138,0xbe,6);
    FUN_2133_0344(0x174,0xc0,4);
    FUN_2133_0344(0x1b0,0xbe,6);
    FUN_2133_0344(0x1ec,0xc0,4);
    FUN_2133_0344(0x228,0xbe,6);
    FUN_24d0_0002(0);
    FUN_2133_02fe(0x137,0xc3,0xf0);
    FUN_2133_0344(0x137,0xbd,6);
    FUN_2133_0344(0x173,0xbf,4);
    FUN_2133_0344(0x1af,0xbd,6);
    FUN_2133_0344(0x1eb,0xbf,4);
    FUN_2133_0344(0x227,0xbd,6);
    FUN_78a8_06ae(2,0xf,0,3);
    sprintf((char *)CONCAT22(unaff_SS,local_20),(char *)0x41250e43,_LAB_4125_0e3f_1);
    FUN_7ecb_0103((int)DAT_4125_0e0c,(int)((ulong)DAT_4125_0e0c >> 0x10),local_20);
    sprintf((char *)CONCAT22(unaff_SS,local_20),(char *)0x41250e46,_LAB_4125_0e3f_1 / 2);
    FUN_7ecb_00d6((int)DAT_4125_0e08,(int)((ulong)DAT_4125_0e08 >> 0x10),local_20);
    FUN_7ecb_00d6((int)DAT_4125_0e04,(int)((ulong)DAT_4125_0e04 >> 0x10),0xe49,0x4125);
    if ((DAT_451b_1679 != 0) && ((DAT_451b_16a4 & 10) == 0)) {
      for (iVar3 = 0; iVar3 < (int)(uint)DAT_451b_16a3; iVar3 = iVar3 + 1) {
        mem_unlock((int)((undefined4 *)&DAT_451b_1679)[iVar3],
                   (int)((ulong)((undefined4 *)&DAT_451b_1679)[iVar3] >> 0x10));
      }
    }
  }
  return;
}


