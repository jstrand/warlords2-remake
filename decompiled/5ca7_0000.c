
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_5ca7_0000(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 local_4;
  
  iVar3 = -1;
  local_4 = 0xffff;
  iVar4 = 0;
  do {
    if (7 < iVar4) {
      _DAT_451b_1efe = 0;
      DAT_451b_1f02 = 0;
      if ((param_1 != 0) && (iVar3 != -1)) {
        FUN_5ca7_10a1(iVar3,local_4);
      }
      refresh_map_owners();
      return;
    }
    iVar1 = army_count;
    if (((undefined2 *)&DAT_451b_1ede)[iVar4 * 2] != 0 ||
        *(int *)((undefined *)&DAT_451b_1ee0 + iVar4 * 4) != 0) {
      do {
        iVar2 = iVar1 + -1;
        if (iVar1 == 0) break;
        iVar1 = iVar2;
      } while ((*(int *)((undefined *)&DAT_451b_1ee0 + iVar4 * 4) != 0x2e1d) ||
              (iVar2 * 0x16 + -0x7780 != ((undefined2 *)&DAT_451b_1ede)[iVar4 * 2]));
      iVar3 = **(int **)((undefined2 *)&DAT_451b_1ede + iVar4 * 2);
      local_4 = *(undefined2 *)(((undefined2 *)&DAT_451b_1ede)[iVar4 * 2] + 2);
      if (iVar2 != -1) {
        FUN_5ca7_015e(iVar2,0);
      }
      *(undefined2 *)((undefined *)&DAT_451b_1ee0 + iVar4 * 4) = 0;
      ((undefined2 *)&DAT_451b_1ede)[iVar4 * 2] = 0;
    }
    iVar4 = iVar4 + 1;
  } while( true );
}


