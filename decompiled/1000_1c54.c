
void __cdecl16far gettime(undefined1 *param_1)

{
  code *pcVar1;
  undefined1 in_CL;
  undefined1 in_CH;
  undefined1 extraout_DL;
  undefined1 extraout_DH;
  undefined1 *puVar2;
  undefined2 uVar3;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  uVar3 = (undefined2)((ulong)param_1 >> 0x10);
  puVar2 = (undefined1 *)param_1;
  *param_1 = in_CH;
  puVar2[1] = in_CL;
  puVar2[2] = extraout_DH;
  puVar2[3] = extraout_DL;
  return;
}


