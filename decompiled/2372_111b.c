
void __cdecl16far FUN_2372_111b(int *param_1)

{
  uint uVar1;
  undefined4 *puVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  byte local_6;
  
  uVar5 = (undefined2)((ulong)DAT_451b_2fc8 >> 0x10);
  puVar2 = (undefined4 *)DAT_451b_2fc8;
  iVar3 = *(int *)((int)puVar2 + 10);
  do {
    iVar4 = iVar3 + -1;
    if (iVar3 == 0) {
      _bios_timeofday(0,(undefined4 *)&DAT_451b_2fc0,0x4125);
      uVar1 = (int)DAT_451b_2fc0 - (int)DAT_451b_2fbc >> 0xf;
      if ((int)(((int)DAT_451b_2fc0 - (int)DAT_451b_2fbc ^ uVar1) - uVar1) < 3) {
        return;
      }
      FUN_2372_102d(0xff);
      DAT_451b_2fbc = DAT_451b_2fc0;
      return;
    }
    iVar3 = iVar4;
  } while (*(int *)((int)puVar2[1] + iVar4 * 8 + 6) != *param_1);
  iVar3 = *(int *)((int)puVar2[1] + iVar4 * 8 + 2);
  FUN_2372_102d(iVar3);
  uVar5 = *(undefined2 *)((int)((undefined4 *)DAT_451b_2fc8)[1] + iVar4 * 8 + 4);
  _bios_timeofday(0,(undefined4 *)&DAT_451b_2fbc,0x4125);
  DAT_451b_2fc0 = DAT_451b_2fbc;
  local_6 = (byte)uVar5;
  if ((*(ulong *)((int)*DAT_451b_2fc8 + iVar3 * 0x2e + 0x1a) >> (local_6 & 0x1f) & 1) == 0) {
    FUN_2372_13e9();
    *param_1 = 0;
  }
  return;
}


