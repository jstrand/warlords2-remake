
char * __cdecl16far FUN_513d_1468(int param_1)

{
  int iVar1;
  char *pcVar2;
  undefined2 unaff_SS;
  char *pcVar3;
  char local_18 [20];
  int local_4;
  
  iVar1 = dice(1,10,0);
  if (iVar1 < 7) {
    local_4 = 3;
  }
  else {
    local_4 = 2;
  }
  strcpy((char *)CONCAT22(unaff_SS,local_18),(char *)0x41250381);
  iVar1 = dice(1,0x14,-1);
  strcpy((char *)CONCAT22(unaff_SS,local_18),
         (char *)CONCAT22(DAT_451b_002a._2_2_,(char *)((int)DAT_451b_002a + iVar1 * 10 + 0xa86)));
  if (local_4 == 3) {
    iVar1 = dice(1,0x14,-1);
    strcat((char *)CONCAT22(unaff_SS,local_18),
           (char *)CONCAT22(DAT_451b_002a._2_2_,(char *)((int)DAT_451b_002a + iVar1 * 10 + 0xb4e)));
  }
  iVar1 = dice(1,10,0);
  if (iVar1 < 7) {
    if (DAT_4125_02de != 0) {
      iVar1 = dice(1,10,-1);
      pcVar2 = (char *)((int)DAT_451b_002a + iVar1 * 10 + 0xda6);
      goto LAB_513d_15ed;
    }
    if (DAT_4125_02d8 != 0) {
      iVar1 = dice(1,10,-1);
      pcVar2 = (char *)((int)DAT_451b_002a + iVar1 * 10 + 0xcde);
      goto LAB_513d_15ed;
    }
    if (DAT_4125_02d6 != 0) {
      iVar1 = dice(1,10,-1);
      pcVar2 = (char *)((int)DAT_451b_002a + iVar1 * 10 + 0xd42);
      goto LAB_513d_15ed;
    }
    if (DAT_4125_02dc != 0) {
      iVar1 = dice(1,10,-1);
      pcVar2 = (char *)((int)DAT_451b_002a + iVar1 * 10 + 0xe0a);
      goto LAB_513d_15ed;
    }
    iVar1 = dice(1,10,0);
    if (iVar1 < 5) {
      iVar1 = dice(1,10,-1);
      pcVar2 = (char *)((int)DAT_451b_002a + iVar1 * 10 + 0xe6e);
      goto LAB_513d_15ed;
    }
  }
  iVar1 = dice(1,0x14,-1);
  pcVar2 = (char *)((int)DAT_451b_002a + iVar1 * 10 + 0xc16);
LAB_513d_15ed:
  strcat((char *)CONCAT22(unaff_SS,local_18),(char *)CONCAT22(DAT_451b_002a._2_2_,pcVar2));
  pcVar3 = strcpy((char *)CONCAT22(0x3c04,(char *)(param_1 * 0x41 + 0x1581)),
                  (char *)CONCAT22(unaff_SS,local_18));
  return (char *)pcVar3;
}


