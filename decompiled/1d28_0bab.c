
void __cdecl16near FUN_1d28_0bab(void)

{
  undefined2 uVar1;
  undefined2 *puVar2;
  undefined2 *in_BX;
  int unaff_SI;
  
  uVar1 = *(undefined2 *)(unaff_SI + 0xe);
  do {
    puVar2 = (undefined2 *)*in_BX;
    *in_BX = 0xffff;
    *(int *)(unaff_SI + 0xc) = *(int *)(unaff_SI + 0xc) + 1;
    in_BX = puVar2;
  } while (puVar2 != (undefined2 *)0xfffe);
  return;
}


