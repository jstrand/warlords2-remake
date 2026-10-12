
void __cdecl16far FUN_2372_102d(int param_1)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 uVar3;
  undefined1 local_e [8];
  int local_6;
  int iStack_4;
  
  if ((param_1 != DAT_451b_300d) && ((param_1 != 0xff || (DAT_451b_300b != 0xff)))) {
    FUN_22bf_07a5();
    DAT_451b_300d = param_1;
    if (DAT_451b_300b != 0xff) {
      iVar2 = *DAT_451b_2fc8 + DAT_451b_300b * 0x2e;
      _local_6 = CONCAT22(((int *)DAT_451b_2fc8)[1],iVar2);
      uVar3 = 7;
      uVar1 = FUN_216d_002e(local_e);
      FUN_2012_044f(0x342a,0x4125,uVar1,iVar2,uVar3);
    }
    if (DAT_451b_300d != 0xff) {
      iVar2 = *DAT_451b_2fc8 + DAT_451b_300d * 0x2e;
      _local_6 = CONCAT22(((int *)DAT_451b_2fc8)[1],iVar2);
      uVar3 = 7;
      uVar1 = FUN_216d_002e(local_e);
      FUN_2012_044f(0x342a,0x4125,uVar1,iVar2,uVar3);
    }
    FUN_22bf_0757();
    DAT_451b_300b = param_1;
  }
  return;
}


