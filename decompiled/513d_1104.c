
int __cdecl16far random_city_value(int city)

{
  int iVar1;
  
  if (*(char *)(city * 0x41 + 0x1592) == '\x0f') {
    iVar1 = dice(1,4,-1);
    if (DAT_4125_02d6 != 0) {
      iVar1 = iVar1 + 4;
    }
    if (DAT_4125_02da != 0) {
      iVar1 = iVar1 + 2;
    }
    if (DAT_4125_02d8 != 0) {
      iVar1 = iVar1 + -1;
    }
    if (DAT_4125_02de != 0) {
      iVar1 = iVar1 + -2;
    }
    if (8 < iVar1) {
      iVar1 = 9;
    }
    if (iVar1 < 1) {
      iVar1 = 0;
    }
  }
  else {
    iVar1 = 9;
  }
  return iVar1;
}


