
void __cdecl16far __gettime_raw(undefined2 *param_1)

{
  code *pcVar1;
  undefined2 in_CX;
  undefined2 extraout_DX;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  *param_1 = in_CX;
  ((undefined2 *)param_1)[1] = extraout_DX;
  return;
}


