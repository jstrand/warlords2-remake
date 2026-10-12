
void __cdecl16far diplomacy_flag_pending(void)

{
  byte *pbVar1;
  bool bVar2;
  bool bVar3;
  int iVar4;
  int iVar5;
  
  iVar4 = current_player;
  bVar2 = false;
  bVar3 = false;
  pbVar1 = (byte *)(current_player * 9 + 0x153b);
  *pbVar1 = *pbVar1 & 0xef;
  pbVar1 = (byte *)(iVar4 * 9 + 0x153b);
  *pbVar1 = *pbVar1 & 0xdf;
  for (iVar5 = 0; iVar5 < 8; iVar5 = iVar5 + 1) {
    if ((iVar5 != iVar4) &&
       ((*(byte *)(iVar5 * 8 + iVar4 + 0x153b) & 3) !=
        (*(byte *)(iVar5 * 8 + iVar4 + 0x153b) & 0xc) >> 2)) {
      if ((*(byte *)(iVar5 * 8 + iVar4 + 0x153b) & 0xc) >> 2 <
          (*(byte *)(iVar5 * 8 + iVar4 + 0x153b) & 3)) {
        bVar2 = true;
      }
      else {
        bVar3 = true;
      }
    }
  }
  if ((bVar2) || (bVar3)) {
    pbVar1 = (byte *)(iVar4 * 9 + 0x153b);
    *pbVar1 = *pbVar1 | 0x10;
  }
  if ((bVar2) && (!bVar3)) {
    pbVar1 = (byte *)(iVar4 * 9 + 0x153b);
    *pbVar1 = *pbVar1 | 0x20;
  }
  return;
}


