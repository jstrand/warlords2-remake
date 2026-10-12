
int __cdecl16far random_magical_type(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = 0;
  for (iVar2 = 0; iVar2 < 0x1c; iVar2 = iVar2 + 1) {
    if (((undefined1 *)&DAT_451b_0668)[iVar2 * 6] != '\0') {
      iVar1 = iVar1 + 1;
    }
  }
  iVar1 = dice(1,iVar1,-1);
  iVar2 = 0;
  for (iVar3 = 0; iVar3 < 0x1c; iVar3 = iVar3 + 1) {
    if (((undefined1 *)&DAT_451b_0668)[iVar3 * 6] != '\0') {
      if (iVar2 == iVar1) break;
      iVar2 = iVar2 + 1;
    }
  }
  if (((0x1b < iVar3) || (iVar3 < 0)) || (((undefined1 *)&DAT_451b_0668)[iVar3 * 6] == '\0')) {
    iVar3 = 0x19;
  }
  return iVar3;
}


