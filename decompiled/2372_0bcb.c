
undefined2 __cdecl16far FUN_2372_0bcb(void)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined1 local_a [8];
  
  uVar3 = DAT_451b_3011;
  if (DAT_451b_300f != 2) {
    iVar5 = ((int *)DAT_451b_2fc8)[4];
    do {
      iVar6 = iVar5 + -1;
      if (iVar5 == 0) {
        return 0xff;
      }
      iVar1 = ((int *)DAT_451b_2fc8)[1];
      iVar2 = *DAT_451b_2fc8;
      FUN_216d_002e(local_a);
      iVar4 = FUN_22bf_08b1(local_a);
      iVar5 = iVar6;
    } while (iVar4 == 0);
    uVar3 = *(undefined2 *)(iVar2 + iVar6 * 0x2e + 8);
  }
  return uVar3;
}


