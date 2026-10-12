
undefined2 __brk(undefined2 param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = (param_2 - DAT_4125_007b) + 0x40U >> 6;
  if (uVar2 != DAT_4125_3ebc) {
    uVar2 = uVar2 * 0x40;
    if (DAT_4125_0091 < uVar2 + DAT_4125_007b) {
      uVar2 = DAT_4125_0091 - DAT_4125_007b;
    }
    iVar1 = setblock(DAT_4125_007b,uVar2);
    if (iVar1 != -1) {
      DAT_4125_008f = 0;
      DAT_4125_0091 = DAT_4125_007b + iVar1;
      return 0;
    }
    DAT_4125_3ebc = uVar2 >> 6;
  }
  DAT_4125_008d = param_2;
  DAT_4125_008b = param_1;
  return 1;
}


