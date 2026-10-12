
void __cdecl16far FUN_54f6_069c(void)

{
  int iVar1;
  
  if (0 < DAT_4125_0636) {
    iVar1 = DAT_4125_0636 + -1;
    FUN_22bf_07a5();
    (*DAT_451b_2a74)(0x22bf,iVar1 * 0x3b + 0x3fdc,0x4125,0,*(undefined2 *)(iVar1 * 8 + 0x3fc8),
                     *(undefined2 *)(iVar1 * 8 + 0x3fca),0x342a,0x4125,
                     *(undefined2 *)(iVar1 * 8 + 0x3fc4),*(undefined2 *)(iVar1 * 8 + 0x3fc6));
    FUN_2012_0975(iVar1 * 0x3b + 0x3fdc,0x4125);
    FUN_22bf_0757();
    DAT_4125_0636 = DAT_4125_0636 + -1;
  }
  return;
}


