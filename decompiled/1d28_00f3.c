
void __cdecl16far FUN_1d28_00f3(void)

{
  code *pcVar1;
  int *piVar2;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  for (piVar2 = (int *)&DAT_4125_3292; *piVar2 != 0; piVar2 = piVar2 + 1) {
    (*(code *)*(undefined2 *)(*piVar2 + 6))();
  }
  DAT_4125_3274 = 0;
  return;
}


