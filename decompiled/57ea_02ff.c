
void __cdecl16far FUN_57ea_02ff(void)

{
  bool bVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int city;
  uint unaff_DI;
  int iVar5;
  undefined2 uVar6;
  uint unaff_SS;
  
  iVar4 = city_count;
  while (city = iVar4 + -1, iVar4 != 0) {
    iVar4 = city;
    if ((*(char *)(city * 0x41 + 0x1592) == (char)current_player) &&
       (*(char *)((int)DAT_451b_018e + city + 0x56) == '\x03')) {
      iVar2 = ai_neutral_neighbours(city,(char *)(ulong)unaff_SS,(char *)((ulong)unaff_DI << 0x10));
      if (iVar2 != 0) {
        bVar1 = false;
        iVar2 = 6;
        while (iVar5 = iVar2 + -1, uVar6 = (undefined2)((ulong)DAT_451b_018e >> 0x10), iVar2 != 0) {
          uVar3 = (uint)(byte)(&stack0xfff7)[iVar2];
          iVar2 = iVar5;
          if (((uVar3 != 0xff) && (*(char *)(uVar3 * 0x41 + 0x1592) == '\x0f')) &&
             ((*(byte *)((int)DAT_451b_018e + uVar3 + 0x11e) & 1) == 0)) {
            bVar1 = true;
          }
        }
        if (!bVar1) {
          *(undefined1 *)((int)DAT_451b_018e + city + 0x56) = 2;
        }
      }
    }
  }
  return;
}


