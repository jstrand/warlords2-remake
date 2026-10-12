
void __cdecl16near FUN_2704_0eba(void)

{
  undefined2 *puVar1;
  undefined2 *puVar2;
  int iVar3;
  undefined2 *puVar4;
  undefined2 *puVar5;
  undefined2 unaff_SS;
  
  puVar5 = (undefined2 *)(s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_ + -0xc);
  puVar4 = (undefined2 *)s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_;
  s_Borland_C_____Copyright_1991_Bor_4125_0004._8_2_ = puVar5;
  for (iVar3 = 5; iVar3 != 0; iVar3 = iVar3 + -1) {
    puVar2 = puVar5;
    puVar5 = puVar5 + 1;
    puVar1 = puVar4;
    puVar4 = puVar4 + 1;
    *puVar2 = *puVar1;
  }
  *(undefined1 *)puVar5 = *(undefined1 *)puVar4;
  return;
}


