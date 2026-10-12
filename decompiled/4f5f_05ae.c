
void __cdecl16far FUN_4f5f_05ae(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  byte bVar5;
  
  for (iVar2 = 0; iVar2 < 0x70; iVar2 = iVar2 + 1) {
    for (iVar3 = 0; iVar3 < 0x9c; iVar3 = iVar3 + 1) {
      if ((((iVar2 != 0) && (iVar3 != 0)) && (iVar2 != 0x6f)) && (iVar3 != 0x9b)) {
        uVar4 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
        iVar1 = (int)DAT_451b_002a;
        if (*(char *)(iVar1 + iVar3 * 0x70 + iVar2 + 0x6120) == '\a') {
          bVar5 = *(char *)(iVar1 + (iVar3 + 1) * 0x70 + iVar2 + 0x6120) == '\x05';
          if (*(char *)(iVar1 + (iVar3 + -1) * 0x70 + iVar2 + 0x6120) == '\x05') {
            bVar5 = bVar5 + 1;
          }
          if (*(char *)(iVar1 + iVar3 * 0x70 + iVar2 + 0x6121) == '\x05') {
            bVar5 = bVar5 + 1;
          }
          if (*(char *)(iVar1 + iVar3 * 0x70 + iVar2 + 0x611f) == '\x05') {
            bVar5 = bVar5 + 1;
          }
          if (2 < bVar5) {
            *(undefined1 *)(iVar1 + iVar3 * 0x70 + iVar2 + 0x6120) = 5;
            FUN_4bed_03aa(iVar2,iVar3);
          }
        }
      }
    }
  }
  return;
}


