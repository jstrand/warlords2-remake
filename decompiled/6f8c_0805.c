
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_6f8c_0805(void)

{
  int iVar1;
  undefined2 uVar2;
  
  FUN_6f8c_076d();
  uVar2 = (undefined2)((ulong)DAT_451b_0504 >> 0x10);
  iVar1 = (int)DAT_451b_0504;
  if (*(char *)(iVar1 + 800) < '\x05') {
    _LAB_4125_0e51_1 = (int)*(char *)(iVar1 + 800);
  }
  else {
    _LAB_4125_0e51_1 = 5;
  }
  DAT_451b_0502 = 0;
  if (*(char *)(iVar1 + 800) + -1 < 4) {
    DAT_451b_0500 = *(char *)(iVar1 + 800) + -1;
  }
  else {
    DAT_451b_0500 = 4;
  }
  _LAB_4125_0e4f_1 = 0;
  FUN_6f8c_07d3();
  FUN_22bf_07a5();
  FUN_6f8c_086e(1);
  FUN_6ef3_026c();
  FUN_22bf_0757();
  return;
}


