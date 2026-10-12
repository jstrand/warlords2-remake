
void __cdecl16far FUN_563e_08b2(int param_1,undefined4 param_2)

{
  char cVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  int iVar7;
  undefined2 uVar8;
  undefined2 uVar9;
  
  iVar7 = 4;
  do {
    iVar2 = iVar7 + -1;
    if (iVar7 == 0) {
      return;
    }
    uVar8 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    iVar7 = iVar2;
  } while (*(int *)((int)DAT_451b_018e + param_1 * 0x5c + iVar2 * 2 + 0x28a) != -1);
  iVar7 = -1;
  cVar1 = -1;
  iVar3 = 8;
  do {
    do {
      iVar4 = iVar3 + -1;
      if (iVar3 == 0) {
LAB_563e_096b:
        if (iVar7 != -1) {
          *(int *)((int)DAT_451b_018e + param_1 * 0x5c + iVar2 * 2 + 0x28a) = iVar7;
        }
        return;
      }
      uVar9 = (undefined2)((ulong)param_2 >> 0x10);
      iVar5 = (int)param_2;
      iVar3 = iVar4;
    } while (*(int *)(iVar5 + iVar4 * 2) == -1);
    if (*(char *)(*(int *)(iVar5 + iVar4 * 2) * 0x16 + -0x777c) == '\x1c') {
      iVar7 = *(int *)(iVar5 + iVar4 * 2);
      goto LAB_563e_096b;
    }
    if (cVar1 < *(char *)(*(int *)(iVar5 + iVar4 * 2) * 0x16 + -0x7778)) {
      piVar6 = (int *)(iVar5 + iVar4 * 2);
      iVar7 = *piVar6;
      cVar1 = *(char *)(*piVar6 * 0x16 + -0x7778);
    }
  } while( true );
}


