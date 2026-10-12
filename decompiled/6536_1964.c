
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_6536_1964(void)

{
  int iVar1;
  undefined2 in_DX;
  undefined2 unaff_SS;
  int local_4;
  
  FUN_54f6_069c();
  iVar1 = armies_at_tile(DAT_451b_01ca,DAT_451b_01c8,(int *)CONCAT22(unaff_SS,&local_4));
  DAT_451b_1f02 = CONCAT22(in_DX,iVar1);
  if (_DAT_451b_1efe != 0) {
    DAT_451b_1f02 = _DAT_451b_1efe;
  }
  if (0 < local_4) {
    FUN_1b62_09ec(local_4,0);
    FUN_8c07_06eb(DAT_451b_1f02);
  }
  FUN_8065_011f(1,0x10000);
  return;
}


