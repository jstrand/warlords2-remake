
void __cdecl16far FUN_513d_05e3(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 local_4;
  
  iVar2 = 0;
  for (local_4 = 0; local_4 < 0x9c; local_4 = local_4 + 1) {
    for (iVar3 = 0; iVar3 < 0x70; iVar3 = iVar3 + 1) {
      if (*(char *)(local_4 * 0xe0 + iVar3 * 2) == '`') {
        *(int *)(iVar2 * 0x41 + 0x157d) = iVar3;
        *(int *)(iVar2 * 0x41 + 0x157f) = local_4;
        *(undefined1 *)(iVar2 * 0x41 + 0x1592) = 0xf;
        iVar1 = dice(1,3,2);
        *(undefined1 *)(iVar2 * 0x41 + 0x1591) = (char)iVar1;
        iVar2 = iVar2 + 1;
      }
    }
  }
  city_count = iVar2;
  return;
}


