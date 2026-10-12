
void __cdecl16far FUN_4eb7_060c(void)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  
  for (iVar4 = 0; iVar4 < 0x70; iVar4 = iVar4 + 1) {
    for (iVar5 = 0; iVar5 < 0x9c; iVar5 = iVar5 + 1) {
      uVar2 = 0;
      uVar6 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      iVar3 = (int)DAT_451b_002a;
      if (((*(char *)(iVar3 + iVar5 * 0x70 + iVar4 + 0x6120) == '\a') ||
          (*(char *)(iVar3 + iVar5 * 0x70 + iVar4 + 0x6120) == '\x05')) ||
         (*(char *)(iVar3 + iVar5 * 0x70 + iVar4 + 0x6120) == '\x06')) {
        if ((*(char *)(iVar3 + (iVar5 + 1) * 0x70 + iVar4 + 0x6120) == '\x03') ||
           (*(char *)(iVar3 + (iVar5 + 1) * 0x70 + iVar4 + 0x6120) == '\x02')) {
          uVar2 = 1;
        }
        if ((*(char *)(iVar3 + (iVar5 + -1) * 0x70 + iVar4 + 0x6120) == '\x03') ||
           (*(char *)(iVar3 + (iVar5 + -1) * 0x70 + iVar4 + 0x6120) == '\x02')) {
          uVar2 = uVar2 + 1;
        }
        if ((*(char *)(iVar3 + iVar5 * 0x70 + iVar4 + 0x6121) == '\x03') ||
           (*(char *)(iVar3 + iVar5 * 0x70 + iVar4 + 0x6121) == '\x02')) {
          uVar2 = uVar2 + 1;
        }
        if ((*(char *)(iVar3 + iVar5 * 0x70 + iVar4 + 0x611f) == '\x03') ||
           (*(char *)(iVar3 + iVar5 * 0x70 + iVar4 + 0x611f) == '\x02')) {
          uVar2 = uVar2 + 1;
        }
        if (2 < uVar2) {
          *(undefined1 *)(iVar3 + iVar5 * 0x70 + iVar4 + 0x6120) = 2;
          FUN_4bed_03aa(iVar4);
        }
      }
    }
  }
  for (iVar4 = 0; iVar4 < 0x70; iVar4 = iVar4 + 1) {
    for (iVar5 = 0; iVar5 < 0x9c; iVar5 = iVar5 + 1) {
      if ((((*(char *)((int)DAT_451b_002a + iVar5 * 0x70 + iVar4 + 0x6120) == '\x03') &&
           (iVar3 = terrain_near(iVar4,iVar5,7), iVar3 == 0)) &&
          (iVar3 = terrain_near(iVar4,iVar5,5), iVar3 == 0)) &&
         (iVar3 = terrain_near(iVar4,iVar5,6), iVar3 == 0)) {
        *(undefined1 *)((int)DAT_451b_002a + iVar5 * 0x70 + iVar4 + 0x6120) = 2;
        FUN_4bed_03aa(iVar4);
      }
    }
  }
  iVar4 = 1;
  do {
    if (0x6e < iVar4) {
      return;
    }
    for (iVar5 = 1; iVar5 < 0x9b; iVar5 = iVar5 + 1) {
      uVar6 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      iVar3 = (int)DAT_451b_002a;
      if ((*(char *)(iVar3 + iVar5 * 0x70 + iVar4 + 0x6120) == '\x02') ||
         (*(char *)(iVar3 + iVar5 * 0x70 + iVar4 + 0x6120) == '\x03')) {
        if (((((*(char *)(iVar3 + (iVar5 + 1) * 0x70 + iVar4 + 0x6121) == '\x02') ||
              (*(char *)(iVar3 + (iVar5 + 1) * 0x70 + iVar4 + 0x6121) == '\x03')) &&
             ((*(char *)(iVar3 + (iVar5 + -1) * 0x70 + iVar4 + 0x611f) == '\x02' ||
              (*(char *)(iVar3 + (iVar5 + -1) * 0x70 + iVar4 + 0x611f) == '\x03')))) &&
            ((*(char *)(iVar3 + (iVar5 + -1) * 0x70 + iVar4 + 0x6121) == '\x05' ||
             ((*(char *)(iVar3 + (iVar5 + -1) * 0x70 + iVar4 + 0x6121) == '\x04' ||
              (*(char *)(iVar3 + (iVar5 + -1) * 0x70 + iVar4 + 0x6121) == '\a')))))) &&
           ((*(char *)(iVar3 + (iVar5 + 1) * 0x70 + iVar4 + 0x611f) == '\x05' ||
            ((*(char *)(iVar3 + (iVar5 + 1) * 0x70 + iVar4 + 0x611f) == '\x04' ||
             (*(char *)(iVar3 + (iVar5 + 1) * 0x70 + iVar4 + 0x611f) == '\a')))))) {
          iVar3 = dice(1,10,0);
          uVar6 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
          if (4 < iVar3) {
            *(undefined1 *)((int)DAT_451b_002a + (iVar5 + 1) * 0x70 + iVar4 + 0x611f) = 3;
            iVar3 = iVar5 + 1;
            goto LAB_4eb7_091c;
          }
          *(undefined1 *)((int)DAT_451b_002a + (iVar5 + -1) * 0x70 + iVar4 + 0x6121) = 3;
          iVar3 = iVar5 + -1;
LAB_4eb7_0a4b:
          iVar1 = iVar4 + 1;
        }
        else {
          if (((((*(char *)(iVar3 + (iVar5 + 1) * 0x70 + iVar4 + 0x611f) != '\x02') &&
                (*(char *)(iVar3 + (iVar5 + 1) * 0x70 + iVar4 + 0x611f) != '\x03')) ||
               ((*(char *)(iVar3 + (iVar5 + -1) * 0x70 + iVar4 + 0x6121) != '\x02' &&
                (*(char *)(iVar3 + (iVar5 + -1) * 0x70 + iVar4 + 0x6121) != '\x03')))) ||
              (((*(char *)(iVar3 + (iVar5 + -1) * 0x70 + iVar4 + 0x611f) != '\x05' &&
                (*(char *)(iVar3 + (iVar5 + -1) * 0x70 + iVar4 + 0x611f) != '\x04')) &&
               (*(char *)(iVar3 + (iVar5 + -1) * 0x70 + iVar4 + 0x611f) != '\a')))) ||
             (((*(char *)(iVar3 + (iVar5 + 1) * 0x70 + iVar4 + 0x6121) != '\x05' &&
               (*(char *)(iVar3 + (iVar5 + 1) * 0x70 + iVar4 + 0x6121) != '\x04')) &&
              (*(char *)(iVar3 + (iVar5 + 1) * 0x70 + iVar4 + 0x6121) != '\a'))))
          goto LAB_4eb7_0a58;
          iVar3 = dice(1,10,0);
          uVar6 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
          if (4 < iVar3) {
            *(undefined1 *)((int)DAT_451b_002a + (iVar5 + 1) * 0x70 + iVar4 + 0x6121) = 3;
            iVar3 = iVar5 + 1;
            goto LAB_4eb7_0a4b;
          }
          *(undefined1 *)((int)DAT_451b_002a + (iVar5 + -1) * 0x70 + iVar4 + 0x611f) = 3;
          iVar3 = iVar5 + -1;
LAB_4eb7_091c:
          iVar1 = iVar4 + -1;
        }
        FUN_4bed_03aa(iVar1,iVar3);
      }
LAB_4eb7_0a58:
    }
    iVar4 = iVar4 + 1;
  } while( true );
}


