
void __cdecl16far FUN_7ecb_09e0(int *param_1,undefined4 param_2)

{
  int iVar1;
  undefined2 uVar2;
  
  iVar1 = 0;
  while (*param_1 = *param_1 + 1, uVar2 = (undefined2)((ulong)param_2 >> 0x10),
        **(char **)param_1 != '|') {
    *(undefined1 *)((int)param_2 + iVar1) = **(undefined1 **)param_1;
    iVar1 = iVar1 + 1;
  }
  *(undefined1 *)((int)param_2 + iVar1) = 0;
  return;
}


