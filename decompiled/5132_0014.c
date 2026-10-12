
void __cdecl16far FUN_5132_0014(void)

{
  int iVar1;
  int iVar2;
  
  for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
    iVar1 = dice(1,5,-1);
    strcpy((char *)CONCAT22(0x3c04,(char *)(iVar2 * 0x14)),
           (char *)CONCAT22(DAT_451b_002a._2_2_,
                            (char *)((int)DAT_451b_002a + iVar2 * 100 + iVar1 * 0x14 + 0x764)));
  }
  return;
}


