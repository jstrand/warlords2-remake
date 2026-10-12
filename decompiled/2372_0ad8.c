
void __cdecl16far FUN_2372_0ad8(int param_1,int param_2)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 uVar3;
  undefined1 local_e [8];
  int local_6;
  int iStack_4;
  
  iStack_4 = ((int *)DAT_451b_2fc8)[1];
  iVar2 = *DAT_451b_2fc8 + param_1 * 0x2e;
  if ((param_2 != DAT_451b_3019) && ((param_2 != 0xff || (DAT_451b_301b != 0xff)))) {
    DAT_451b_3019 = param_2;
    local_6 = iVar2;
    FUN_22bf_07a5();
    if (DAT_451b_301b != 0xff) {
      uVar3 = 0xf;
      uVar1 = FUN_216d_002e(local_e);
      FUN_2012_044f(0x342a,0x4125,uVar1,iVar2,uVar3);
    }
    if (DAT_451b_3019 != 0xff) {
      uVar3 = 0xf;
      uVar1 = FUN_216d_002e(local_e);
      FUN_2012_044f(0x342a,0x4125,uVar1,iVar2,uVar3);
    }
    FUN_22bf_0757();
    DAT_451b_301b = param_2;
  }
  return;
}


