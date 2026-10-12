
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __cdecl16far FUN_229d_000e(void)

{
  int iVar1;
  undefined2 uVar2;
  int iVar3;
  
  _DAT_451b_2e4a = 0;
  iVar3 = 0;
  do {
    if (4 < iVar3) {
LAB_229d_0055:
      uVar2 = DAT_451b_2e4a;
      return uVar2;
    }
    if (((undefined1 *)&DAT_4125_355a)[iVar3] == '\0') {
      iVar1 = iVar3 * 0x14;
      _DAT_451b_2e4a = CONCAT22(0x4125,iVar1 + 0x6ea8);
      *(undefined2 *)(iVar1 + 0x6eb2) = 0x4125;
      *(int *)(iVar1 + 0x6eb0) = iVar3 * 0x32 + 0x6dae;
      ((undefined1 *)&DAT_4125_355a)[iVar3] = 1;
      goto LAB_229d_0055;
    }
    iVar3 = iVar3 + 1;
  } while( true );
}


