
void __cdecl16far FUN_8611_2756(undefined4 param_1)

{
  uint *puVar1;
  byte *pbVar2;
  byte bVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  undefined2 uVar10;
  int local_4;
  
  if (opt_hidden_map != 0) {
    FUN_8611_13c8(0xffffffff);
  }
  iVar4 = 0;
  for (iVar9 = DAT_3c04_0177; iVar9 <= DAT_3c04_017b; iVar9 = iVar9 + 1) {
    iVar8 = 0;
    for (local_4 = DAT_3c04_0175; local_4 <= DAT_3c04_0179; local_4 = local_4 + 1) {
      uVar5 = (uint)*(byte *)(local_4 * 0xe0 + iVar9 * 2);
      iVar7 = (int)param_1;
      uVar10 = (undefined2)((ulong)param_1 >> 0x10);
      if (uVar5 < 0x60) {
        iVar6 = iVar7 + iVar4 * 0x48 + iVar8 * 8;
        puVar1 = (uint *)(iVar6 + 0x16);
        *puVar1 = *puVar1 & 0xfc03;
        puVar1 = (uint *)(iVar6 + 0x16);
        *puVar1 = *puVar1 | uVar5 << 2;
      }
      else {
        iVar6 = iVar7 + iVar4 * 0x48 + iVar8 * 8;
        puVar1 = (uint *)(iVar6 + 0x16);
        *puVar1 = *puVar1 & 0xfc03;
        puVar1 = (uint *)(iVar6 + 0x16);
        *puVar1 = *puVar1 | (uVar5 - 0x60 & 0xff) << 2;
        pbVar2 = (byte *)(iVar7 + iVar4 * 0x48 + iVar8 * 8 + 0x16);
        *pbVar2 = *pbVar2 | 2;
        *(byte *)(iVar7 + 0x29e) = *(byte *)(iVar7 + 0x29e) | 2;
      }
      if (*(char *)(local_4 * 0xe0 + iVar9 * 2 + 1) < '\0') {
        pbVar2 = (byte *)(iVar7 + iVar4 * 0x48 + iVar8 * 8 + 0x19);
        *pbVar2 = *pbVar2 | 0x20;
        *(byte *)(iVar7 + 0x29e) = *(byte *)(iVar7 + 0x29e) | 0x10;
      }
      if ((*(byte *)(local_4 * 0xe0 + iVar9 * 2 + 1) & 0x20) != 0) {
        pbVar2 = (byte *)(iVar7 + iVar4 * 0x48 + iVar8 * 8 + 0x1b);
        *pbVar2 = *pbVar2 | 8;
        *(byte *)(iVar7 + 0x29e) = *(byte *)(iVar7 + 0x29e) | 4;
      }
      if ((*(byte *)(local_4 * 0x70 + iVar9) & 0x1f) != 0) {
        pbVar2 = (byte *)(iVar7 + iVar4 * 0x48 + iVar8 * 8 + 0x1a);
        *pbVar2 = *pbVar2 | 0x20;
        bVar3 = *(byte *)(local_4 * 0x70 + iVar9);
        iVar6 = iVar7 + iVar4 * 0x48 + iVar8 * 8;
        puVar1 = (uint *)(iVar6 + 0x1a);
        *puVar1 = *puVar1 & 0xf83f;
        puVar1 = (uint *)(iVar6 + 0x1a);
        *puVar1 = *puVar1 | ((bVar3 & 0x1f) - 1 & 0x1f) << 6;
        *(byte *)(iVar7 + 0x29e) = *(byte *)(iVar7 + 0x29e) | 8;
      }
      if (-1 < (char)((undefined1 *)&DAT_451b_1f74)[iVar4 * 9 + iVar8]) {
        pbVar2 = (byte *)(iVar7 + iVar4 * 0x48 + iVar8 * 8 + 0x19);
        *pbVar2 = *pbVar2 | 0x80;
        bVar3 = ((undefined1 *)&DAT_451b_1f74)[iVar4 * 9 + iVar8];
        iVar7 = iVar7 + iVar4 * 0x48 + iVar8 * 8;
        pbVar2 = (byte *)(iVar7 + 0x1a);
        *pbVar2 = *pbVar2 & 0xe0;
        pbVar2 = (byte *)(iVar7 + 0x1a);
        *pbVar2 = *pbVar2 | bVar3 & 0x1f;
      }
      iVar8 = iVar8 + 1;
    }
    iVar4 = iVar4 + 1;
  }
  return;
}


