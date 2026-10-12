
void __cdecl16far hero_check_promotions(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  
  iVar2 = army_count;
  while( true ) {
    do {
      iVar3 = iVar2 + -1;
      if (iVar2 == 0) {
        return;
      }
      iVar2 = iVar3;
    } while ((*(char *)(iVar3 * 0x16 + -0x777c) != '\x1c') ||
            (*(char *)(iVar3 * 0x16 + -0x777b) != current_player));
    iVar1 = (int)*(char *)(iVar3 * 0x16 + -0x7776);
    if (((*(byte *)(iVar1 + 0x5e3) & 3) == 0) && (0xe < *(byte *)(iVar1 + 0x5e3) >> 2)) break;
    if (((*(byte *)(iVar1 + 0x5e3) & 3) == 1) && (0x1d < *(byte *)(iVar1 + 0x5e3) >> 2)) {
      uVar4 = 0x110066;
      goto LAB_7563_065b;
    }
    if (((*(byte *)(iVar1 + 0x5e3) & 3) == 2) && (0x3b < *(byte *)(iVar1 + 0x5e3) >> 2)) {
      uVar4 = 0x120067;
LAB_7563_065b:
      hero_promote(iVar3,uVar4);
    }
  }
  uVar4 = 0x100065;
  goto LAB_7563_065b;
}


