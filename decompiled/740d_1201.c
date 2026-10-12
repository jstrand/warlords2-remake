
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_740d_1201(void)

{
  int iVar1;
  undefined2 uVar2;
  undefined2 in_DX;
  undefined2 unaff_SS;
  undefined4 local_a;
  undefined4 local_6;
  
  FUN_740d_131a();
  iVar1 = FUN_2012_01fe(0x4494,0x4125,local_6);
  if (iVar1 != 0) {
    FUN_22bf_07a5();
    if ((DAT_451b_053e != 0) && ((DAT_451b_0569 & 10) == 0)) {
      for (iVar1 = 0; iVar1 < (int)(uint)DAT_451b_0568; iVar1 = iVar1 + 1) {
        uVar2 = mem_lock(((undefined4 *)&DAT_451b_053e)[iVar1]);
        ((undefined2 *)&DAT_451b_0550)[iVar1 * 2] = in_DX;
        ((undefined2 *)&DAT_451b_054e)[iVar1 * 2] = uVar2;
      }
    }
    (*DAT_451b_2a74)();
    if ((DAT_451b_053e != 0) && ((DAT_451b_0569 & 10) == 0)) {
      for (iVar1 = 0; iVar1 < (int)(uint)DAT_451b_0568; iVar1 = iVar1 + 1) {
        mem_unlock(((undefined4 *)&DAT_451b_053e)[iVar1]);
      }
    }
    _DAT_451b_0530 = local_a;
    __F_SCOPY(&local_a,unaff_SS,(undefined2 *)&DAT_451b_0528,0x4125);
    FUN_1997_034a();
    (*DAT_451b_2a68)(0x1997,0x54a8,0x4125,_LAB_4125_1047_1,_LAB_4125_104b_1,0x342a,0x4125,
                     _DAT_451b_0530,0x546d,0x4125,_LAB_4125_1047_1);
  }
  return;
}


