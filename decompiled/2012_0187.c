
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_2012_0187(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  DAT_451b_2a56 = param_1;
  iVar3 = DAT_451b_2a56;
  DAT_451b_2a56._0_1_ = (byte)param_1;
  _DAT_451b_2a54 = 1 << ((byte)DAT_451b_2a56 & 0x1f);
  iVar2 = _DAT_451b_2a54;
  _DAT_451b_2a52 = _DAT_451b_2a54 * *(int *)((int)DAT_4125_34e7 + 0xe);
  iVar1 = _DAT_451b_2a52;
  DAT_451b_2a42 = (int)(0x10 / (long)_DAT_451b_2a52);
  DAT_451b_2a4c = (1 << (DAT_451b_2a54 & 0x1f)) + -1;
  DAT_451b_2a4a = 0x10 >> ((byte)DAT_451b_2a56 & 0x1f);
  DAT_451b_2a48 = DAT_451b_2a4a >> 1;
  DAT_451b_2a46 = DAT_451b_2a4a + -1;
  DAT_451b_2a44 = (DAT_451b_2a4a >> 1) + -1;
  _DAT_451b_2a40 = -param_1 + 4;
  _DAT_451b_2a3e = -param_1 + 3;
  DAT_451b_2a4e = 1 << (DAT_451b_2a52 & 0x1f);
  DAT_451b_2a50 = DAT_451b_2a4e + -1;
  _DAT_451b_2a52 = iVar1;
  _DAT_451b_2a54 = iVar2;
  DAT_451b_2a56 = iVar3;
  return;
}


