
void __cdecl16far army_add_strength(undefined4 param_1,char param_2)

{
  int iVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  iVar1 = (int)param_1;
  *(char *)(iVar1 + 8) = *(char *)(iVar1 + 8) + param_2;
  if ('\t' < *(char *)(iVar1 + 8)) {
    *(undefined1 *)(iVar1 + 8) = 9;
  }
  return;
}


