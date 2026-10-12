
void __cdecl16far FUN_4bed_036b(void)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  
  for (iVar3 = 0; iVar3 < 0x70; iVar3 = iVar3 + 1) {
    for (iVar2 = 0; iVar2 < 0x9c; iVar2 = iVar2 + 1) {
      *(undefined1 *)((int)DAT_451b_002a + iVar2 * 0x70 + iVar3 + 0x6120) = 2;
      pbVar1 = (byte *)(iVar2 * 0x70 + iVar3);
      *pbVar1 = *pbVar1 & 0xe0;
    }
  }
  return;
}


