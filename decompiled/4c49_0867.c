
void __cdecl16far FUN_4c49_0867(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 local_4;
  
  for (iVar2 = 0; iVar2 < 0x70; iVar2 = iVar2 + 1) {
    for (iVar3 = 0; iVar3 < 0x9c; iVar3 = iVar3 + 1) {
      uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      iVar1 = (int)DAT_451b_002a;
      if (*(char *)(iVar1 + iVar3 * 0x70 + iVar2 + 0x6120) == '\a') {
        *(undefined1 *)(iVar1 + iVar3 * 0x70 + iVar2 + 0x6120) = 3;
      }
      else {
        *(undefined1 *)(iVar1 + iVar3 * 0x70 + iVar2 + 0x6120) = 0;
      }
    }
  }
  for (local_4 = 0; local_4 < 2; local_4 = local_4 + 1) {
    for (iVar2 = 0; iVar2 < 0x70; iVar2 = iVar2 + 1) {
      for (iVar3 = 0; iVar3 < 0x9c; iVar3 = iVar3 + 1) {
        iVar1 = FUN_4c49_10bd(iVar2,iVar3);
        if (iVar1 != 0) {
          *(undefined1 *)((int)DAT_451b_002a + iVar3 * 0x70 + iVar2 + 0x6120) = 2;
        }
      }
    }
    for (iVar2 = 0; iVar2 < 0x9c; iVar2 = iVar2 + 1) {
      for (iVar3 = 0; iVar3 < 0x70; iVar3 = iVar3 + 1) {
        iVar1 = FUN_4c49_10bd(iVar3,iVar2);
        if (iVar1 != 0) {
          *(undefined1 *)((int)DAT_451b_002a + iVar2 * 0x70 + iVar3 + 0x6120) = 2;
        }
      }
    }
    iVar2 = 0x70;
    while (iVar3 = iVar2 + -1, iVar2 != 0) {
      iVar1 = 0x9c;
      while (iVar4 = iVar1 + -1, iVar2 = iVar3, iVar1 != 0) {
        iVar2 = FUN_4c49_10bd(iVar3,iVar4);
        iVar1 = iVar4;
        if (iVar2 != 0) {
          *(undefined1 *)((int)DAT_451b_002a + iVar4 * 0x70 + iVar3 + 0x6120) = 2;
        }
      }
    }
    iVar2 = 0x9c;
    while (iVar3 = iVar2 + -1, iVar2 != 0) {
      iVar1 = 0x70;
      while (iVar4 = iVar1 + -1, iVar2 = iVar3, iVar1 != 0) {
        iVar2 = FUN_4c49_10bd(iVar4,iVar3);
        iVar1 = iVar4;
        if (iVar2 != 0) {
          *(undefined1 *)((int)DAT_451b_002a + iVar3 * 0x70 + iVar4 + 0x6120) = 2;
        }
      }
    }
  }
  for (iVar2 = 0; iVar2 < 0x70; iVar2 = iVar2 + 1) {
    for (iVar3 = 0; iVar3 < 0x9c; iVar3 = iVar3 + 1) {
      uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      iVar1 = (int)DAT_451b_002a;
      if (*(char *)(iVar1 + iVar3 * 0x70 + iVar2 + 0x6120) == '\x02') {
        *(undefined1 *)(iVar1 + iVar3 * 0x70 + iVar2 + 0x6120) = 2;
      }
      else {
        *(undefined1 *)(iVar1 + iVar3 * 0x70 + iVar2 + 0x6120) = 7;
      }
    }
  }
  FUN_4bed_0449();
  return;
}


