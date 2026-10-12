
void __cdecl16far FUN_4fef_0464(void)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  byte *pbVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  
  for (iVar7 = 0; iVar7 < 0x9c; iVar7 = iVar7 + 1) {
    for (iVar6 = 0; iVar6 < 0x70; iVar6 = iVar6 + 1) {
      uVar8 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      if ((*(char *)((int)DAT_451b_002a + iVar7 * 0x70 + iVar6 + 0x6120) == '\x02') ||
         (*(char *)((int)DAT_451b_002a + iVar7 * 0x70 + iVar6 + 0x6120) == '\x03')) {
        iVar2 = FUN_4fef_0c9d();
        if ((iVar6 + iVar7 == 0) || ((iVar6 + iVar7) / 2 != (iVar6 + iVar7 + -1) / 2)) {
          iVar3 = 1;
        }
        else {
          iVar3 = 0;
        }
        uVar8 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
        iVar4 = (int)DAT_451b_002a;
        if (*(char *)(iVar4 + iVar2 + 0x264) < '\0') {
          *(undefined1 *)(iVar4 + iVar7 * 0x70 + iVar6 + 0x6120) = 8;
          uVar8 = *(undefined2 *)((int)DAT_451b_002a + iVar3 * 2 + 0x664);
          pbVar5 = (byte *)(iVar7 * 0xe0 + iVar6 * 2);
          *pbVar5 = 0;
          pbVar1 = pbVar5;
          *pbVar1 = *pbVar1 | (byte)uVar8;
          FUN_4fc9_0029();
          pbVar1 = (byte *)(iVar7 * 0xe0 + iVar6 * 2 + 1);
          *pbVar1 = *pbVar1 & 0x7f;
        }
        else {
          *(undefined1 *)(iVar4 + iVar7 * 0x70 + iVar6 + 0x6120) = 2;
          uVar8 = *(undefined2 *)
                   ((int)DAT_451b_002a + *(char *)((int)DAT_451b_002a + iVar2 + 0x264) * 4 +
                    iVar3 * 2 + 0x4e4);
          pbVar5 = (byte *)(iVar7 * 0xe0 + iVar6 * 2);
          *pbVar5 = 0;
          pbVar1 = pbVar5;
          *pbVar1 = *pbVar1 | (byte)uVar8;
        }
      }
    }
  }
  for (iVar7 = 0; iVar7 < 0x9c; iVar7 = iVar7 + 1) {
    for (iVar6 = 0; iVar6 < 0x70; iVar6 = iVar6 + 1) {
      if (*(char *)((int)DAT_451b_002a + iVar7 * 0x70 + iVar6 + 0x6120) == '\x04') {
        iVar2 = FUN_4fef_0d4f();
        if ((iVar6 + iVar7 == 0) || ((iVar6 + iVar7) / 2 != (iVar6 + iVar7 + -1) / 2)) {
          iVar3 = 1;
        }
        else {
          iVar3 = 0;
        }
        uVar8 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
        iVar4 = (int)DAT_451b_002a;
        if (*(char *)(iVar4 + iVar2 + 0x264) < '\0') {
          iVar2 = dice(1,10,0);
          uVar8 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
          if (iVar2 < 6) {
            uVar8 = *(undefined2 *)((int)DAT_451b_002a + iVar3 * 2 + 0x624);
            pbVar5 = (byte *)(iVar7 * 0xe0 + iVar6 * 2);
            *pbVar5 = 0;
            pbVar1 = pbVar5;
            *pbVar1 = *pbVar1 | (byte)uVar8;
            *(undefined1 *)((int)DAT_451b_002a + iVar7 * 0x70 + iVar6 + 0x6120) = 7;
          }
          else {
            uVar8 = *(undefined2 *)((int)DAT_451b_002a + iVar3 * 2 + 0x598);
            pbVar5 = (byte *)(iVar7 * 0xe0 + iVar6 * 2);
            *pbVar5 = 0;
            pbVar1 = pbVar5;
            *pbVar1 = *pbVar1 | (byte)uVar8;
          }
        }
        else {
          uVar8 = *(undefined2 *)(iVar4 + *(char *)(iVar4 + iVar2 + 0x264) * 4 + iVar3 * 2 + 0x564);
          pbVar5 = (byte *)(iVar7 * 0xe0 + iVar6 * 2);
          *pbVar5 = 0;
          pbVar1 = pbVar5;
          *pbVar1 = *pbVar1 | (byte)uVar8;
          *(undefined1 *)((int)DAT_451b_002a + iVar7 * 0x70 + iVar6 + 0x6120) = 4;
        }
      }
    }
  }
  for (iVar7 = 0; iVar7 < 0x9c; iVar7 = iVar7 + 1) {
    for (iVar6 = 0; iVar6 < 0x70; iVar6 = iVar6 + 1) {
      if (*(char *)((int)DAT_451b_002a + iVar7 * 0x70 + iVar6 + 0x6120) == '\x06') {
        iVar2 = FUN_4fef_0dcb();
        if ((iVar6 + iVar7 == 0) || ((iVar6 + iVar7) / 2 != (iVar6 + iVar7 + -1) / 2)) {
          iVar3 = 1;
        }
        else {
          iVar3 = 0;
        }
        uVar8 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
        iVar4 = (int)DAT_451b_002a;
        if (*(char *)(iVar4 + iVar2 + 0x264) < '\0') {
          uVar8 = *(undefined2 *)(iVar4 + iVar3 * 2 + 0x5a4);
          pbVar5 = (byte *)(iVar7 * 0xe0 + iVar6 * 2);
          *pbVar5 = 0;
          pbVar1 = pbVar5;
          *pbVar1 = *pbVar1 | (byte)uVar8;
          *(undefined1 *)((int)DAT_451b_002a + iVar7 * 0x70 + iVar6 + 0x6120) = 5;
        }
        else {
          uVar8 = *(undefined2 *)(iVar4 + *(char *)(iVar4 + iVar2 + 0x264) * 4 + iVar3 * 2 + 0x5e4);
          pbVar5 = (byte *)(iVar7 * 0xe0 + iVar6 * 2);
          *pbVar5 = 0;
          pbVar1 = pbVar5;
          *pbVar1 = *pbVar1 | (byte)uVar8;
          *(undefined1 *)((int)DAT_451b_002a + iVar7 * 0x70 + iVar6 + 0x6120) = 6;
        }
      }
    }
  }
  for (iVar7 = 0; iVar7 < 0x9c; iVar7 = iVar7 + 1) {
    for (iVar6 = 0; iVar6 < 0x70; iVar6 = iVar6 + 1) {
      if (*(char *)((int)DAT_451b_002a + iVar7 * 0x70 + iVar6 + 0x6120) == '\x05') {
        iVar2 = FUN_4fef_0e47();
        if ((iVar6 + iVar7 == 0) || ((iVar6 + iVar7) / 2 != (iVar6 + iVar7 + -1) / 2)) {
          iVar3 = 1;
        }
        else {
          iVar3 = 0;
        }
        uVar8 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
        iVar4 = (int)DAT_451b_002a;
        if (*(char *)(iVar4 + iVar2 + 0x264) < '\0') {
          iVar2 = dice(1,10,0);
          uVar8 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
          if (iVar2 < 6) {
            uVar8 = *(undefined2 *)((int)DAT_451b_002a + iVar3 * 2 + 0x624);
            pbVar5 = (byte *)(iVar7 * 0xe0 + iVar6 * 2);
            *pbVar5 = 0;
            pbVar1 = pbVar5;
            *pbVar1 = *pbVar1 | (byte)uVar8;
            *(undefined1 *)((int)DAT_451b_002a + iVar7 * 0x70 + iVar6 + 0x6120) = 7;
          }
          else {
            *(undefined1 *)((int)DAT_451b_002a + iVar7 * 0x70 + iVar6 + 0x6120) = 5;
            uVar8 = *(undefined2 *)((int)DAT_451b_002a + iVar3 * 2 + 0x5d8);
            pbVar5 = (byte *)(iVar7 * 0xe0 + iVar6 * 2);
            *pbVar5 = 0;
            pbVar1 = pbVar5;
            *pbVar1 = *pbVar1 | (byte)uVar8;
          }
        }
        else {
          uVar8 = *(undefined2 *)(iVar4 + *(char *)(iVar4 + iVar2 + 0x264) * 4 + iVar3 * 2 + 0x5a4);
          pbVar5 = (byte *)(iVar7 * 0xe0 + iVar6 * 2);
          *pbVar5 = 0;
          pbVar1 = pbVar5;
          *pbVar1 = *pbVar1 | (byte)uVar8;
          *(undefined1 *)((int)DAT_451b_002a + iVar7 * 0x70 + iVar6 + 0x6120) = 5;
        }
      }
    }
  }
  for (iVar7 = 0; iVar7 < 0x9c; iVar7 = iVar7 + 1) {
    for (iVar6 = 0; iVar6 < 0x70; iVar6 = iVar6 + 1) {
      if (*(char *)((int)DAT_451b_002a + iVar7 * 0x70 + iVar6 + 0x6120) == '\b') {
        if ((iVar6 + iVar7 == 0) || ((iVar6 + iVar7) / 2 != (iVar6 + iVar7 + -1) / 2)) {
          iVar2 = 1;
        }
        else {
          iVar2 = 0;
        }
        iVar3 = dice(1,10,0);
        if (iVar3 < 7) {
          iVar2 = iVar2 << 1;
        }
        else {
          iVar2 = dice(1,4,0);
          iVar2 = iVar2 << 2;
        }
        uVar8 = *(undefined2 *)((int)DAT_451b_002a + iVar2 + 0x664);
        pbVar5 = (byte *)(iVar7 * 0xe0 + iVar6 * 2);
        *pbVar5 = 0;
        pbVar1 = pbVar5;
        *pbVar1 = *pbVar1 | (byte)uVar8;
        *(undefined1 *)((int)DAT_451b_002a + iVar7 * 0x70 + iVar6 + 0x6120) = 8;
      }
    }
  }
  return;
}


