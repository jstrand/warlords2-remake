
void __cdecl16far FUN_5f19_058a(int param_1)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  undefined2 *puVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  undefined2 unaff_SS;
  undefined2 local_10;
  byte abStack_9 [7];
  
  iVar6 = city_count;
  while( true ) {
    uVar8 = (undefined2)((ulong)DAT_451b_01a2 >> 0x10);
    if (iVar6 == 0) break;
    puVar4 = (undefined2 *)((int)DAT_451b_01a2 + (iVar6 + -1) * 2);
    puVar4[200] = 0;
    puVar4[100] = 0;
    *puVar4 = 0;
    iVar6 = iVar6 + -1;
  }
  *(undefined2 *)((int)DAT_451b_01a2 + param_1 * 2) = 1;
  do {
    local_10 = 0;
    iVar6 = city_count;
LAB_5f19_0689:
    iVar7 = iVar6 + -1;
    if (iVar6 != 0) {
      uVar8 = (undefined2)((ulong)DAT_451b_01a2 >> 0x10);
      piVar5 = (int *)((int)DAT_451b_01a2 + iVar7 * 2);
      iVar6 = iVar7;
      if ((*piVar5 != 0) && (piVar5[200] == 0)) {
        local_10 = 1;
        iVar1 = piVar5[100];
        piVar5[200] = 1;
        FUN_5db9_0656(iVar7,0,abStack_9 + 1);
        iVar7 = 6;
LAB_5f19_0682:
        do {
          iVar3 = iVar7;
          iVar7 = iVar3 + -1;
          if (iVar3 == 0) break;
          uVar2 = (uint)abStack_9[iVar3];
          if (uVar2 != 0xff) {
            iVar3 = iVar1 + (uint)*(byte *)((int)&local_10 + iVar3 + 1);
            uVar8 = (undefined2)((ulong)DAT_451b_01a2 >> 0x10);
            piVar5 = (int *)((int)DAT_451b_01a2 + uVar2 * 2);
            if (*piVar5 == 0) {
              *piVar5 = 1;
            }
            else if (*(int *)((int)DAT_451b_01a2 + uVar2 * 2 + 200) <= iVar3) goto LAB_5f19_0682;
            *(int *)((int)DAT_451b_01a2 + uVar2 * 2 + 200) = iVar3;
          }
        } while( true );
      }
      goto LAB_5f19_0689;
    }
    if (local_10 == 0) {
      return;
    }
  } while( true );
}


