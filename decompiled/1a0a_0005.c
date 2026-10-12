
void __cdecl16far FUN_1a0a_0005(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  if (*(long *)((int)param_1 + 0xc) == 0) {
    FUN_1771_0001(param_1);
  }
  else {
    FUN_1a0a_02be((int)param_1,uVar3);
  }
  if ((DAT_4125_166a != 0) && (iVar2 = FUN_17be_0037(), iVar2 != 0)) {
    FUN_24d0_0002();
    FUN_216d_002e(&local_a);
    FUN_2133_02fe(local_a + 1,local_8,local_6 + -2);
    FUN_2133_0344(local_a + local_6 + -1,local_8,2);
    FUN_2133_02fe(local_a + local_6 + -1,local_8 + 1,2);
    FUN_2133_0344(local_a + local_6,local_8 + 1,local_4 + -2);
    FUN_2133_02fe(local_a + local_6 + -1,local_8 + local_4 + -1,2);
    FUN_2133_0344(local_a + local_6 + -1,local_8 + local_4 + -1,2);
    FUN_2133_02fe(local_a + 1);
    FUN_2133_0344(local_a + 1,local_8 + local_4 + -1,2);
    FUN_2133_02fe(local_a,local_8 + local_4 + -1);
    FUN_2133_0344(local_a,local_8 + 1,local_4 + -2);
    FUN_2133_02fe(local_a);
    FUN_2133_0344(local_a + 1,local_8,2);
    iVar1 = local_6;
    iVar2 = local_a;
    local_a = local_a + -1;
    local_8 = local_8 + -1;
    local_6 = local_6 + 2;
    local_4 = local_4 + 2;
    FUN_2133_02fe(iVar2,local_8,iVar1);
    FUN_2133_0344(local_a + local_6 + -1,local_8,2);
    FUN_2133_02fe(local_a + local_6 + -1,local_8 + 1,2);
    FUN_2133_0344(local_a + local_6,local_8 + 1,local_4 + -2);
    FUN_2133_02fe(local_a + local_6 + -1,local_8 + local_4 + -1,2);
    FUN_2133_0344(local_a + local_6 + -1,local_8 + local_4 + -1,2);
    FUN_2133_02fe(local_a + 1,local_8 + local_4,local_6 + -2);
    FUN_2133_0344(local_a + 1,local_8 + local_4 + -1,2);
    FUN_2133_02fe(local_a,local_8 + local_4 + -1,2);
    FUN_2133_0344(local_a,local_8 + 1,local_4 + -2);
    FUN_2133_02fe(local_a,local_8 + 1,2);
    FUN_2133_0344(local_a + 1,local_8,2);
  }
  return;
}


