
void __cdecl16far FUN_5132_006b(void)

{
  int iVar1;
  int iVar2;
  
  for (iVar2 = 0; iVar2 < 8; iVar2 = iVar2 + 1) {
    iVar1 = dice(3,0x32,0x14);
    *(int *)(iVar2 * 0x14 + 0x185) = iVar1;
  }
  return;
}


