
void __cdecl16far diplomacy_init(void)

{
  byte *pbVar1;
  undefined1 *puVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  for (iVar4 = 0; iVar4 < 8; iVar4 = iVar4 + 1) {
    for (iVar3 = 0; iVar3 < 8; iVar3 = iVar3 + 1) {
      if (opt_diplomacy == 0) {
        iVar5 = iVar4 * 8 + iVar3;
        pbVar1 = (byte *)(iVar5 + 0x153b);
        *pbVar1 = *pbVar1 & 0xf3;
        pbVar1 = (byte *)(iVar5 + 0x153b);
        *pbVar1 = *pbVar1 | 8;
        iVar5 = iVar4 * 8 + iVar3;
        pbVar1 = (byte *)(iVar5 + 0x153b);
        *pbVar1 = *pbVar1 & 0xfc;
        pbVar1 = (byte *)(iVar5 + 0x153b);
        *pbVar1 = *pbVar1 | 2;
      }
      else {
        pbVar1 = (byte *)(iVar4 * 8 + iVar3 + 0x153b);
        *pbVar1 = *pbVar1 & 0xf3;
        iVar5 = iVar4 * 8 + iVar3;
        pbVar1 = (byte *)(iVar5 + 0x153b);
        *pbVar1 = *pbVar1 & 0xfc;
        puVar2 = (undefined1 *)(iVar5 + 0x153b);
        *puVar2 = *puVar2;
      }
      pbVar1 = (byte *)(iVar4 * 8 + iVar3 + 0x153b);
      *pbVar1 = *pbVar1 & 0xef;
      pbVar1 = (byte *)(iVar4 * 8 + iVar3 + 0x153b);
      *pbVar1 = *pbVar1 & 0xdf;
    }
  }
  return;
}


