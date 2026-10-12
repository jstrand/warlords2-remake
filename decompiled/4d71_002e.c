
void __cdecl16far random_map_smooth_highlands(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  
  for (iVar3 = 0; iVar3 < 0x70; iVar3 = iVar3 + 1) {
    for (iVar4 = 0; iVar4 < 0x9c; iVar4 = iVar4 + 1) {
      if ((((iVar3 != 0) && (iVar4 != 0)) && (iVar3 != 0x6f)) && (iVar4 != 0x9b)) {
        uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
        iVar1 = (int)DAT_451b_002a;
        if (*(char *)(iVar1 + iVar4 * 0x70 + iVar3 + 0x6120) == '\a') {
          if (((*(char *)(iVar1 + (iVar4 + 1) * 0x70 + iVar3 + 0x6120) == '\x06') &&
              (*(char *)(iVar1 + iVar4 * 0x70 + iVar3 + 0x6121) == '\x06')) &&
             ((*(char *)(iVar1 + iVar4 * 0x70 + iVar3 + 0x611f) == '\x06' &&
              (*(char *)(iVar1 + (iVar4 + -1) * 0x70 + iVar3 + 0x6120) == '\x06')))) {
            *(undefined1 *)(iVar1 + iVar4 * 0x70 + iVar3 + 0x6120) = 6;
            FUN_4bed_03aa(iVar3);
          }
          uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
          iVar1 = (int)DAT_451b_002a;
          if (((*(char *)(iVar1 + (iVar4 + 1) * 0x70 + iVar3 + 0x6120) == '\x05') &&
              (*(char *)(iVar1 + iVar4 * 0x70 + iVar3 + 0x6121) == '\x05')) &&
             ((*(char *)(iVar1 + iVar4 * 0x70 + iVar3 + 0x611f) == '\x05' &&
              (*(char *)(iVar1 + (iVar4 + -1) * 0x70 + iVar3 + 0x6120) == '\x05')))) {
            *(undefined1 *)(iVar1 + iVar4 * 0x70 + iVar3 + 0x6120) = 5;
            FUN_4bed_03aa(iVar3);
          }
        }
      }
      if ((*(char *)((int)DAT_451b_002a + iVar4 * 0x70 + iVar3 + 0x6120) == '\x06') &&
         (iVar1 = terrain_near(iVar3,iVar4,3), iVar1 != 0)) {
        *(undefined1 *)((int)DAT_451b_002a + iVar4 * 0x70 + iVar3 + 0x6120) = 7;
        FUN_4bed_03aa(iVar3);
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
      iVar1 = (int)DAT_451b_002a;
      if (*(char *)(iVar1 + iVar4 * 0x70 + iVar3 + 0x6120) == '\x06') {
        if ((((*(char *)(iVar1 + (iVar4 + 1) * 0x70 + iVar3 + 0x6121) == '\x06') &&
             (*(char *)(iVar1 + (iVar4 + -1) * 0x70 + iVar3 + 0x611f) == '\x06')) &&
            (*(char *)(iVar1 + (iVar4 + -1) * 0x70 + iVar3 + 0x6121) != '\x06')) &&
           (*(char *)(iVar1 + (iVar4 + 1) * 0x70 + iVar3 + 0x611f) != '\x06')) {
          iVar1 = dice(1,10,0);
          uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
          if (5 < iVar1) {
            *(undefined1 *)((int)DAT_451b_002a + (iVar4 + 1) * 0x70 + iVar3 + 0x611f) = 6;
            iVar1 = iVar4 + 1;
            goto LAB_4d71_0271;
          }
          *(undefined1 *)((int)DAT_451b_002a + (iVar4 + -1) * 0x70 + iVar3 + 0x6121) = 6;
          iVar1 = iVar4 + -1;
LAB_4d71_0316:
          iVar2 = iVar3 + 1;
        }
        else {
          if (((*(char *)(iVar1 + (iVar4 + 1) * 0x70 + iVar3 + 0x611f) != '\x06') ||
              (*(char *)(iVar1 + (iVar4 + -1) * 0x70 + iVar3 + 0x6121) != '\x06')) ||
             ((*(char *)(iVar1 + (iVar4 + -1) * 0x70 + iVar3 + 0x611f) == '\x06' ||
              (*(char *)(iVar1 + (iVar4 + 1) * 0x70 + iVar3 + 0x6121) == '\x06'))))
          goto LAB_4d71_0323;
          iVar1 = dice(1,10,0);
          uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
          if (5 < iVar1) {
            *(undefined1 *)((int)DAT_451b_002a + (iVar4 + 1) * 0x70 + iVar3 + 0x6121) = 6;
            iVar1 = iVar4 + 1;
            goto LAB_4d71_0316;
          }
          *(undefined1 *)((int)DAT_451b_002a + (iVar4 + -1) * 0x70 + iVar3 + 0x611f) = 6;
          iVar1 = iVar4 + -1;
LAB_4d71_0271:
          iVar2 = iVar3 + -1;
        }
        FUN_4bed_03aa(iVar2,iVar1);
      }
LAB_4d71_0323:
    }
    iVar3 = iVar3 + 1;
  } while( true );
}


