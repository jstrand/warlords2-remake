
void __cdecl16far FUN_25de_0144(void)

{
  code *pcVar1;
  undefined2 uVar2;
  undefined2 in_BX;
  undefined2 unaff_ES;
  undefined2 in_stack_00000000;
  undefined2 in_stack_00000002;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)(in_BX);
  *(undefined2 *)&DAT_4125_0002 = in_stack_00000000;
  *(undefined2 *)(char *)s_Borland_C_____Copyright_1991_Bor_4125_0004 = in_stack_00000002;
  if (*(int *)0x128 == 0) {
    pcVar1 = (code *)swi(0x21);
    uVar2 = (*pcVar1)();
    *(undefined2 *)0x128 = uVar2;
  }
  else {
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    *(undefined2 *)0x128 = 0;
  }
  return;
}


