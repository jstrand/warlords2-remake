
void __cdecl16far load_hero_name(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  void *pvVar4;
  int iVar5;
  uint unaff_DI;
  int iVar6;
  char *fmt;
  long lVar7;
  undefined2 uVar8;
  
  uVar8 = DAT_3c04_0161;
  fmt = get_file_string(0x14,current_player);
  sprintf(&DAT_451b_1d28,fmt,uVar8);
  iVar1 = open(&DAT_451b_1d28,2);
  lVar7 = filelength(iVar1);
  pvVar4 = (void *)((ulong)lVar7 >> 0x10);
  iVar2 = (int)lVar7;
  iVar3 = FUN_1d28_01d5(iVar2);
  read(iVar1,(void *)CONCAT22(iVar2,pvVar4),unaff_DI);
  close(iVar1);
  iVar6 = 0;
  iVar1 = dice(1,100,0);
  for (iVar5 = 0; iVar5 < iVar2; iVar5 = iVar5 + 1) {
    if ((*(char *)(iVar3 + iVar5) == '#') && (iVar6 = iVar6 + 1, iVar6 == iVar1)) {
      iVar5 = iVar5 + 1;
      break;
    }
  }
  DAT_451b_0570 = (uint)(*(char *)(iVar3 + iVar5) != '0');
  iVar1 = iVar5 + 2;
  do {
    if (iVar2 <= iVar1) {
LAB_7563_0d4c:
      FUN_1d28_050d(iVar3,pvVar4);
      return;
    }
    if (*(char *)(iVar3 + iVar1) < ' ') {
      *(undefined1 *)(iVar3 + iVar1) = 0;
      strcpy((char *)0x412544d2,(char *)CONCAT22(pvVar4,(char *)(iVar3 + iVar5 + 2)));
      goto LAB_7563_0d4c;
    }
    iVar1 = iVar1 + 1;
  } while( true );
}


