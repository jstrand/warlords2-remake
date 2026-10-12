
void __cdecl16far segread(undefined2 *param_1)

{
  undefined2 *puVar1;
  undefined2 unaff_ES;
  undefined2 uVar2;
  undefined2 unaff_SS;
  undefined2 in_stack_00000002;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  puVar1 = (undefined2 *)param_1;
  *param_1 = unaff_ES;
  puVar1[1] = in_stack_00000002;
  puVar1[2] = unaff_SS;
  puVar1[3] = 0x4125;
  return;
}


