
void __cdecl16far FUN_5311_01f6(undefined4 param_1)

{
  byte *pbVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  int *piVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  undefined2 uVar11;
  undefined2 uVar12;
  int local_4;
  
  local_4 = 0;
  iVar10 = 1;
  while( true ) {
    iVar8 = (int)param_1;
    uVar12 = (undefined2)((ulong)param_1 >> 0x10);
    if (0x99 < iVar10) break;
    for (iVar9 = 1; iVar9 < 0x6e; iVar9 = iVar9 + 1) {
      iVar3 = dice(1,2,-1);
      if (iVar3 == 0) {
        bVar2 = false;
        uVar6 = *(byte *)(iVar10 * 0xe0 + iVar9 * 2) - 0x20;
        if (uVar6 < 6) {
                    /* WARNING: Switch is manually overridden */
          iVar3 = (int)DAT_451b_002a;
          uVar11 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
          switch((undefined4)*(undefined2 *)(uVar6 * 2 + 0x3b7)) {
          case 0x53364:
            bVar2 = true;
            iVar4 = 3;
            while (iVar5 = iVar4 + -1, iVar4 != 0) {
              iVar4 = iVar5;
              if (*(char *)(iVar3 + iVar10 * 0x70 + (iVar9 - iVar5) + 0x611f) != '\x02') {
                bVar2 = false;
              }
            }
            break;
          case 0x53390:
            bVar2 = true;
            iVar4 = 3;
            while (iVar5 = iVar4 + -1, iVar4 != 0) {
              iVar4 = iVar5;
              if (*(char *)(iVar3 + ((iVar10 - iVar5) + -1) * 0x70 + iVar9 + 0x6120) != '\x02') {
                bVar2 = false;
              }
            }
            break;
          case 0x533bb:
            bVar2 = true;
            iVar4 = 3;
            while (iVar5 = iVar4 + -1, iVar4 != 0) {
              iVar4 = iVar5;
              if (*(char *)(iVar3 + iVar10 * 0x70 + iVar9 + iVar5 + 0x6121) != '\x02') {
                bVar2 = false;
              }
            }
            break;
          case 0x533e5:
            bVar2 = true;
            iVar4 = 3;
            while (iVar5 = iVar4 + -1, iVar4 != 0) {
              iVar4 = iVar5;
              if (*(char *)(iVar3 + (iVar10 + iVar5 + 1) * 0x70 + iVar9 + 0x6120) != '\x02') {
                bVar2 = false;
              }
            }
          }
        }
        if ((bVar2) && (iVar3 = FUN_5311_0565(param_1,iVar9,iVar10), 0xe < iVar3)) {
          iVar3 = 0;
          while ((iVar4 = -1, iVar3 < 0x32 && (iVar4 = iVar3, *(int *)(iVar8 + iVar3 * 2) != -1))) {
            iVar3 = iVar3 + 1;
          }
          if (iVar4 != -1) {
            piVar7 = (int *)(iVar8 + iVar4 * 2);
            *piVar7 = iVar9;
            piVar7[0x32] = iVar10;
          }
        }
      }
    }
    iVar10 = iVar10 + 1;
  }
  *(undefined2 *)(iVar8 + 400) = 0xffff;
  while ((iVar10 = FUN_5311_03c3(param_1), iVar10 != -1 && (local_4 < 10))) {
    local_4 = local_4 + 1;
    piVar7 = (int *)(iVar8 + iVar10 * 2);
    piVar7[0x96] = 1;
    pbVar1 = (byte *)(piVar7[0x32] * 0xe0 + *piVar7 * 2 + 1);
    *pbVar1 = *pbVar1 | 0x80;
  }
  return;
}


