
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __cdecl16far FUN_5db9_0656(int param_1,uint param_2,long param_3,long param_4)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  
  uVar3 = 0;
  auto_str_no_room_ai_data();
  iVar1 = 6;
  while (iVar2 = iVar1 + -1, iVar1 != 0) {
    uVar4 = (undefined2)((ulong)_DAT_451b_018a >> 0x10);
    if (*(byte *)((int)_DAT_451b_018a + param_1 * 6 + iVar2) == param_2) {
      uVar3 = 1;
    }
    if (param_3 != 0) {
      *(undefined1 *)((int)param_3 + iVar2) =
           *(undefined1 *)((int)_DAT_451b_018a + param_1 * 6 + iVar2);
    }
    iVar1 = iVar2;
    if (param_4 != 0) {
      *(undefined1 *)((int)param_4 + iVar2) =
           *(undefined1 *)((int)_DAT_451b_018a + param_1 * 6 + iVar2 + 600);
    }
  }
  FUN_5db9_063d();
  return uVar3;
}


