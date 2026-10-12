
void __cdecl16far
FUN_6087_1a3d(undefined4 param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  int *piVar1;
  int iVar2;
  bool bVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  undefined2 uVar9;
  undefined2 uVar10;
  undefined2 uVar11;
  
  bVar3 = false;
  do {
    if (bVar3) {
      return;
    }
    bVar3 = true;
    for (iVar5 = 0; iVar5 < 8; iVar5 = iVar5 + 1) {
      uVar9 = (undefined2)((ulong)param_1 >> 0x10);
      if (*(int *)((int)param_1 + iVar5 * 2) != -1) {
        uVar10 = (undefined2)((ulong)param_2 >> 0x10);
        iVar7 = (int)param_2;
        if ('\x01' < *(char *)(iVar7 + iVar5)) {
          for (iVar8 = 0; iVar8 < 8; iVar8 = iVar8 + 1) {
            if (((iVar5 != iVar8) && (iVar2 = *(int *)((int)param_1 + iVar8 * 2), iVar2 != -1)) &&
               (*(char *)(iVar7 + iVar8) == '\0')) {
              for (iVar6 = 0; iVar6 < 0x16; iVar6 = iVar6 + 1) {
                uVar11 = (undefined2)((ulong)param_3 >> 0x10);
                iVar4 = (int)*(char *)((int)param_3 + iVar5 * 0x16 + iVar6);
                if ((iVar4 != 0xff) && (*(char *)(iVar4 * 0x1d + 0xcfd) == param_4)) {
                  bVar3 = false;
                  *(undefined1 *)((int)param_3 + iVar5 * 0x16 + iVar6) = 0xff;
                  piVar1 = (int *)((int)DAT_451b_018e + 0x4c);
                  *piVar1 = *piVar1 + 1;
                  *(int *)(iVar4 * 0x1d + 0xd00) = iVar2;
                  *(char *)(iVar7 + iVar5) = *(char *)(iVar7 + iVar5) + -1;
                  *(char *)(iVar7 + iVar8) = *(char *)(iVar7 + iVar8) + '\x01';
                  break;
                }
              }
            }
          }
        }
      }
    }
  } while( true );
}


