
void __cdecl16near FUN_2704_0e87(void)

{
  undefined2 *puVar1;
  undefined2 *puVar2;
  int iVar3;
  undefined2 *unaff_SI;
  undefined2 *puVar4;
  
  puVar4 = (undefined2 *)(s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_ + -0xc);
  s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_ = puVar4;
  for (iVar3 = 5; iVar3 != 0; iVar3 = iVar3 + -1) {
    puVar2 = puVar4;
    puVar4 = puVar4 + 1;
    puVar1 = unaff_SI;
    unaff_SI = unaff_SI + 1;
    *puVar2 = *puVar1;
  }
  *(undefined1 *)puVar4 = *(undefined1 *)unaff_SI;
  return;
}


