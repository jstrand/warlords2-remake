
void __cdecl16far FUN_255e_061e(int param_1)

{
  if (((DAT_4125_37a2 != '\0') && (DAT_4125_37ab == '\0')) &&
     (param_1 = param_1 * 0xb, ((undefined1 *)&DAT_451b_3108)[param_1] != '\0')) {
    ((undefined1 *)&DAT_451b_3108)[param_1] = 0;
    FUN_1d28_048e(*(undefined2 *)((int)(undefined2 *)&DAT_451b_310f + param_1),
                  *(undefined2 *)((int)(undefined2 *)&DAT_451b_3111 + param_1));
  }
  return;
}


