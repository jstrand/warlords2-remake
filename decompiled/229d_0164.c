
void __cdecl16far FUN_229d_0164(undefined4 param_1,undefined4 param_2)

{
  long lVar1;
  long lVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined2 uVar7;
  
  iVar6 = (int)((ulong)param_2 >> 0x10);
  iVar3 = (int)param_2;
  lVar1 = *(long *)(iVar3 + 0x10);
  lVar2 = *(long *)(iVar3 + 0xc);
  iVar4 = (int)param_1;
  uVar7 = (undefined2)((ulong)param_1 >> 0x10);
  if (lVar1 == 0) {
    *(long *)(iVar4 + 6) = lVar2;
  }
  else {
    *(long *)((int)lVar1 + 0xc) = lVar2;
  }
  if (lVar2 == 0) {
    *(long *)(iVar4 + 10) = lVar1;
  }
  else {
    *(long *)((int)lVar2 + 0x10) = lVar1;
  }
  iVar5 = 0;
  do {
    if (4 < iVar5) {
LAB_229d_01e8:
      *(int *)(iVar4 + 4) = *(int *)(iVar4 + 4) + -1;
      return;
    }
    if ((iVar6 == 0x4125) && (iVar5 * 0x14 + 0x6ea8 == iVar3)) {
      ((undefined1 *)&DAT_4125_355a)[iVar5] = 0;
      goto LAB_229d_01e8;
    }
    iVar5 = iVar5 + 1;
  } while( true );
}


