
int __cdecl16far city_slot_of_type(int city,int type)

{
  int iVar1;
  int iVar2;
  
  iVar1 = 0;
  iVar2 = 0;
  while ((iVar1 < 4 && (*(char *)(city * 0x41 + iVar1 + 0x1593) != type))) {
    iVar2 = iVar2 + 1;
    iVar1 = iVar1 + 1;
  }
  if (3 < iVar2) {
    iVar2 = 0;
  }
  return iVar2;
}


