
void __cdecl16far FUN_5db9_0481(int param_1)

{
  int iVar1;
  int iVar2;
  
  iVar1 = 8;
  while (iVar2 = iVar1 + -1, iVar1 != 0) {
    iVar1 = iVar2;
    if (*(int *)(iVar2 * 2 + 0x137) != 0) {
      *(uint *)(iVar2 * 2 + 0xd0) = (uint)(param_1 != 0);
    }
  }
  return;
}


