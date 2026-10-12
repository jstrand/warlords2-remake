
int __cdecl16far FUN_18a9_0756(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined1 local_e [8];
  int local_6;
  int iStack_4;
  
  iVar1 = FUN_229d_0224();
  if (iVar1 == 0) {
LAB_18a9_07e8:
    iVar2 = 0;
  }
  else {
    iVar1 = **(int **)((int *)DAT_451b_0ce0 + 4);
    do {
      do {
        iVar2 = iVar1 + -1;
        if (iVar1 == 0) goto LAB_18a9_07e8;
        iVar3 = *DAT_451b_0ce0 + iVar2 * 0x21;
        _local_6 = CONCAT22(((int *)DAT_451b_0ce0)[1],iVar3);
        iVar1 = iVar2;
      } while (*(char *)(iVar3 + 5) == '\0');
      FUN_216d_002e(local_e);
      FUN_24b0_0137(local_e);
      iVar3 = FUN_22bf_08b1(local_e);
    } while (iVar3 == 0);
  }
  return iVar2;
}


