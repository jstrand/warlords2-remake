
void __cdecl16far FUN_4e47_03d9(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  byte bVar6;
  
  for (iVar3 = 0; iVar3 < 0x70; iVar3 = iVar3 + 1) {
    for (iVar4 = 0; iVar4 < 0x9c; iVar4 = iVar4 + 1) {
      uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      iVar2 = (int)DAT_451b_002a;
      if (*(char *)(iVar2 + iVar4 * 0x70 + iVar3 + 0x6120) == '\a') {
        bVar6 = *(char *)(iVar2 + (iVar4 + 1) * 0x70 + iVar3 + 0x6120) == '\x04';
        if (*(char *)(iVar2 + (iVar4 + -1) * 0x70 + iVar3 + 0x6120) == '\x04') {
          bVar6 = bVar6 + 1;
        }
        if (*(char *)(iVar2 + iVar4 * 0x70 + iVar3 + 0x6121) == '\x04') {
          bVar6 = bVar6 + 1;
        }
        if (*(char *)(iVar2 + iVar4 * 0x70 + iVar3 + 0x611f) == '\x04') {
          bVar6 = bVar6 + 1;
        }
        if (2 < bVar6) {
          *(undefined1 *)(iVar2 + iVar4 * 0x70 + iVar3 + 0x6120) = 4;
          *(int *)((int)DAT_451b_002a + 0x1f52) = *(int *)((int)DAT_451b_002a + 0x1f52) + 1;
          FUN_4bed_03aa(iVar3,iVar4);
        }
      }
    }
  }
  iVar3 = 1;
  do {
    if (0x6e < iVar3) {
      return;
    }
    for (iVar4 = 1; iVar4 < 0x9b; iVar4 = iVar4 + 1) {
      uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      iVar2 = (int)DAT_451b_002a;
      if (*(char *)(iVar2 + iVar4 * 0x70 + iVar3 + 0x6120) == '\x04') {
        if ((((*(char *)(iVar2 + (iVar4 + 1) * 0x70 + iVar3 + 0x6121) == '\x04') &&
             (*(char *)(iVar2 + (iVar4 + -1) * 0x70 + iVar3 + 0x611f) == '\x04')) &&
            ((*(char *)(iVar2 + (iVar4 + -1) * 0x70 + iVar3 + 0x6121) == '\x05' ||
             ((*(char *)(iVar2 + (iVar4 + -1) * 0x70 + iVar3 + 0x6121) == '\x03' ||
              (*(char *)(iVar2 + (iVar4 + -1) * 0x70 + iVar3 + 0x6121) == '\a')))))) &&
           ((*(char *)(iVar2 + (iVar4 + 1) * 0x70 + iVar3 + 0x611f) == '\x05' ||
            ((*(char *)(iVar2 + (iVar4 + 1) * 0x70 + iVar3 + 0x611f) == '\x03' ||
             (*(char *)(iVar2 + (iVar4 + 1) * 0x70 + iVar3 + 0x611f) == '\a')))))) {
          iVar2 = dice(1,10,0);
          uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
          if (iVar2 < 5) {
            *(undefined1 *)((int)DAT_451b_002a + (iVar4 + -1) * 0x70 + iVar3 + 0x6121) = 4;
            iVar2 = iVar4 + -1;
LAB_4e47_06bd:
            iVar1 = iVar3 + 1;
          }
          else {
            *(undefined1 *)((int)DAT_451b_002a + (iVar4 + 1) * 0x70 + iVar3 + 0x611f) = 4;
            iVar2 = iVar4 + 1;
LAB_4e47_05ba:
            iVar1 = iVar3 + -1;
          }
          FUN_4bed_03aa(iVar1,iVar2);
        }
        else if (((*(char *)(iVar2 + (iVar4 + 1) * 0x70 + iVar3 + 0x611f) == '\x04') &&
                 (*(char *)(iVar2 + (iVar4 + -1) * 0x70 + iVar3 + 0x6121) == '\x04')) &&
                ((((*(char *)(iVar2 + (iVar4 + -1) * 0x70 + iVar3 + 0x611f) == '\x05' ||
                   (*(char *)(iVar2 + (iVar4 + -1) * 0x70 + iVar3 + 0x611f) == '\x03')) ||
                  (*(char *)(iVar2 + (iVar4 + -1) * 0x70 + iVar3 + 0x611f) == '\a')) &&
                 (((*(char *)(iVar2 + (iVar4 + 1) * 0x70 + iVar3 + 0x6121) == '\x05' ||
                   (*(char *)(iVar2 + (iVar4 + 1) * 0x70 + iVar3 + 0x6121) == '\x03')) ||
                  (*(char *)(iVar2 + (iVar4 + 1) * 0x70 + iVar3 + 0x6121) == '\a')))))) {
          iVar2 = dice(1,10,0);
          uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
          if (iVar2 < 5) {
            *(undefined1 *)((int)DAT_451b_002a + (iVar4 + -1) * 0x70 + iVar3 + 0x611f) = 4;
            iVar2 = iVar4 + -1;
            goto LAB_4e47_05ba;
          }
          *(undefined1 *)((int)DAT_451b_002a + (iVar4 + 1) * 0x70 + iVar3 + 0x6121) = 4;
          iVar2 = iVar4 + 1;
          goto LAB_4e47_06bd;
        }
      }
    }
    iVar3 = iVar3 + 1;
  } while( true );
}


