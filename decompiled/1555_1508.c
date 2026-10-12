
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __cdecl16far FUN_1555_1508(undefined4 param_1,int param_2)

{
  int iVar1;
  undefined2 in_DX;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  
  uVar3 = 0;
  DAT_451b_0012._0_2_ = mem_lock((int)DAT_451b_001e,(int)((ulong)DAT_451b_001e >> 0x10));
  DAT_451b_0012._2_2_ = in_DX;
  DAT_451b_001a = mem_lock((int)DAT_451b_0026,(int)((ulong)DAT_451b_0026 >> 0x10));
  uVar4 = (undefined2)((ulong)param_1 >> 0x10);
  _DAT_451b_001c = in_DX;
  if (param_2 == 0) {
    iVar2 = FUN_1555_1595((int)param_1,uVar4,0);
    if (iVar2 != 0) {
      uVar3 = 1;
    }
  }
  else {
    for (iVar2 = 1; iVar2 < 0x14; iVar2 = iVar2 + 1) {
      iVar1 = FUN_1555_1595((int)param_1,uVar4,iVar2);
      if (iVar1 != 0) {
        uVar3 = 1;
        break;
      }
    }
  }
  mem_unlock((int)DAT_451b_0026,(int)((ulong)DAT_451b_0026 >> 0x10));
  mem_unlock((int)DAT_451b_001e,(int)((ulong)DAT_451b_001e >> 0x10));
  return uVar3;
}


