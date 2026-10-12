
void __cdecl16far FUN_5311_05b4(undefined4 param_1)

{
  byte *pbVar1;
  bool bVar2;
  uint uVar3;
  int iVar4;
  int *piVar5;
  byte *pbVar6;
  int iVar7;
  int iVar8;
  undefined2 uVar9;
  undefined2 uVar10;
  int local_8;
  
  local_8 = 0;
  iVar8 = 1;
  while( true ) {
    uVar9 = (undefined2)((ulong)param_1 >> 0x10);
    if (0x99 < iVar8) break;
    for (iVar7 = 1; iVar7 < 0x6e; iVar7 = iVar7 + 1) {
      if ((((*(char *)(iVar8 * 0xe0 + iVar7 * 2) == '&') ||
           (*(char *)(iVar8 * 0xe0 + iVar7 * 2) == '\x16')) &&
          ((*(char *)(iVar8 * 0xe0 + iVar7 * 2 + 2) == '(' ||
           (*(char *)(iVar8 * 0xe0 + iVar7 * 2 + 2) == '\x18')))) ||
         (((*(char *)(iVar8 * 0xe0 + iVar7 * 2) == '!' ||
           (*(char *)(iVar8 * 0xe0 + iVar7 * 2) == '\x11')) &&
          ((*(char *)(iVar8 * 0xe0 + iVar7 * 2 + 0xe0) == '$' ||
           (*(char *)(iVar8 * 0xe0 + iVar7 * 2 + 0xe0) == '\x14')))))) {
        bVar2 = false;
        uVar3 = (uint)(*(char *)(iVar8 * 0xe0 + iVar7 * 2) == '&');
        if ((((uVar3 != 0) && (iVar4 = FUN_5311_0a7f(), iVar4 != 0)) &&
            (iVar4 = FUN_5311_0a7f(), iVar4 != 0)) ||
           (((uVar3 != 0 && (iVar4 = FUN_5311_0a7f(), iVar4 != 0)) &&
            (iVar4 = FUN_5311_0a7f(), iVar4 != 0)))) {
          bVar2 = true;
        }
        if ((bVar2) && (local_8 < 0x32)) {
          piVar5 = (int *)((int)param_1 + local_8 * 2);
          *piVar5 = iVar7;
          piVar5[0x32] = iVar8;
          piVar5[100] = uVar3;
          local_8 = local_8 + 1;
        }
      }
    }
    iVar8 = iVar8 + 1;
  }
  if (local_8 != 0) {
    while (iVar8 = FUN_5311_08f6(param_1,local_8), iVar8 != -1) {
      piVar5 = (int *)((int)param_1 + iVar8 * 2);
      piVar5[0x96] = 1;
      iVar8 = *piVar5;
      iVar7 = piVar5[0x32];
      if (piVar5[100] == 0) {
        pbVar6 = (byte *)(iVar7 * 0xe0 + iVar8 * 2);
        *pbVar6 = 0;
        pbVar1 = pbVar6;
        *pbVar1 = *pbVar1 | 0x84;
        iVar4 = iVar7 * 0xe0 + iVar8 * 2;
        *(undefined1 *)(iVar4 + 0xe0) = 0;
        pbVar1 = (byte *)(iVar4 + 0xe0);
        *pbVar1 = *pbVar1 | 0x94;
        uVar10 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
        *(undefined1 *)((int)DAT_451b_002a + (iVar7 + 1) * 0x70 + iVar8 + 0x6120) = 1;
        *(undefined1 *)((int)DAT_451b_002a + iVar7 * 0x70 + iVar8 + 0x6120) = 1;
        iVar4 = iVar7 * 0x70 + iVar8;
        pbVar1 = (byte *)(iVar4 + 0xe0);
        *pbVar1 = *pbVar1 & 0xe0;
        pbVar1 = (byte *)(iVar4 + 0xe0);
        *pbVar1 = *pbVar1 | 1;
        iVar8 = iVar7 * 0x70 + iVar8;
        pbVar1 = (byte *)(iVar8 + -0x70);
        *pbVar1 = *pbVar1 & 0xe0;
        pbVar1 = (byte *)(iVar8 + -0x70);
        *pbVar1 = *pbVar1 | 1;
      }
      else {
        pbVar6 = (byte *)(iVar7 * 0xe0 + iVar8 * 2);
        *pbVar6 = 0;
        pbVar1 = pbVar6;
        *pbVar1 = *pbVar1 | 0x85;
        iVar4 = iVar7 * 0xe0 + iVar8 * 2;
        *(undefined1 *)(iVar4 + 2) = 0;
        pbVar1 = (byte *)(iVar4 + 2);
        *pbVar1 = *pbVar1 | 0x86;
        uVar10 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
        *(undefined1 *)((int)DAT_451b_002a + iVar7 * 0x70 + iVar8 + 0x6121) = 1;
        *(undefined1 *)((int)DAT_451b_002a + iVar7 * 0x70 + iVar8 + 0x6120) = 1;
        iVar4 = iVar7 * 0x70 + iVar8;
        pbVar1 = (byte *)(iVar4 + 2);
        *pbVar1 = *pbVar1 & 0xe0;
        pbVar1 = (byte *)(iVar4 + 2);
        *pbVar1 = *pbVar1 | 1;
        iVar8 = iVar7 * 0x70 + iVar8;
        pbVar1 = (byte *)(iVar8 + -1);
        *pbVar1 = *pbVar1 & 0xe0;
        pbVar1 = (byte *)(iVar8 + -1);
        *pbVar1 = *pbVar1 | 1;
      }
    }
  }
  return;
}


