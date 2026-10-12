
void __cdecl16far ai_city_seen_check(int param_1)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  bool bVar4;
  int iVar5;
  int iVar6;
  
  bVar4 = false;
  iVar2 = *(int *)(param_1 * 0x41 + 0x157d);
  iVar3 = *(int *)(param_1 * 0x41 + 0x157f);
  for (iVar6 = iVar2 + -1; iVar6 <= iVar2 + 2; iVar6 = iVar6 + 1) {
    for (iVar5 = iVar3 + -1; iVar5 <= iVar3 + 2; iVar5 = iVar5 + 1) {
      if ((*(byte *)(iVar5 * 0x70 + iVar6) & 0x20) != 0) {
        bVar4 = true;
      }
    }
  }
  if (bVar4) {
    pbVar1 = (byte *)((int)DAT_451b_018e + param_1 + 0x11e);
    *pbVar1 = *pbVar1 & 0xfe;
  }
  return;
}


