
void __cdecl16near FUN_25de_0763(void)

{
  uint in_AX;
  uint uVar1;
  uint in_DX;
  uint *puVar2;
  uint *unaff_BP;
  undefined2 unaff_SS;
  
  puVar2 = (uint *)0x0;
  while( true ) {
    uVar1 = *unaff_BP >> 1;
    if (uVar1 == 0) break;
    if ((((*unaff_BP & 1) == 0) && (in_DX == unaff_BP[2])) &&
       (unaff_BP[2] = in_AX, puVar2 == (uint *)0x0)) {
      puVar2 = unaff_BP;
    }
    unaff_BP = (uint *)(uVar1 << 1);
  }
  return;
}


