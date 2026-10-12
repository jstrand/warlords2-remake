
void __cdecl16far sea_change_spend_moves(void)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  undefined2 uVar6;
  int *piVar5;
  
  iVar2 = 8;
  piVar4 = (int *)&DAT_451b_1efe;
  while( true ) {
    piVar5 = piVar4 + -2;
    iVar3 = iVar2 + -1;
    if (iVar2 == 0) break;
    piVar1 = piVar4 + -1;
    iVar2 = iVar3;
    piVar4 = piVar5;
    if (*piVar5 != 0 || *piVar1 != 0) {
      if (DAT_451b_1e64 == 1) {
        uVar6 = (undefined2)((ulong)*(undefined4 *)piVar5 >> 0x10);
        if ((*(uint *)((int)*(undefined4 *)piVar5 + 0xc) & 0x1000) != 0) {
          *(undefined1 *)(*piVar5 + 7) = 0;
        }
      }
      else if ((DAT_451b_1e64 == 2) &&
              (uVar6 = (undefined2)((ulong)*(undefined4 *)piVar5 >> 0x10),
              (*(uint *)((int)*(undefined4 *)piVar5 + 0xc) & 0x1000) == 0)) {
        *(undefined1 *)(*piVar5 + 7) = 0;
      }
    }
  }
  return;
}


