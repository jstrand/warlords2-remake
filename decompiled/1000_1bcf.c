
void __cdecl16far _bios_timeofday(char param_1,undefined2 *param_2)

{
  code *pcVar1;
  undefined2 uVar2;
  undefined2 extraout_DX;
  undefined2 *puVar3;
  undefined2 uVar4;
  
  uVar4 = (undefined2)((ulong)param_2 >> 0x10);
  puVar3 = (undefined2 *)param_2;
  uVar2 = puVar3[1];
  pcVar1 = (code *)swi(0x1a);
  (*pcVar1)();
  if (param_1 == '\0') {
    *puVar3 = extraout_DX;
    puVar3[1] = uVar2;
  }
  return;
}


