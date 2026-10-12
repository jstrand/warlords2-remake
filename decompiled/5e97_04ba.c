
void __cdecl16far ai_phase_quick_attack(void)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined2 uVar7;
  
  iVar5 = city_count;
  if (*(int *)((int)DAT_451b_018e + 0x48) == 0) {
    while (iVar6 = iVar5 + -1, iVar5 != 0) {
      iVar5 = iVar6;
      if (*(char *)(iVar6 * 0x41 + 0x1592) == current_player) {
        bVar1 = false;
        iVar3 = 0;
        while( true ) {
          uVar7 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
          if (*(int *)(iVar3 * 2 + 0x932) == -1) break;
          iVar2 = iVar3 + 1;
          iVar4 = iVar3 * 2;
          iVar3 = iVar2;
          if ((uint)*(byte *)((int)DAT_451b_018e + iVar6 + 0x56) == *(uint *)(iVar4 + 0x932)) {
            bVar1 = true;
          }
        }
        if ((bVar1) && (3 < *(byte *)((int)DAT_451b_018e + iVar6 + 0x182))) {
          FUN_5e97_053f(iVar6);
        }
      }
    }
  }
  return;
}


