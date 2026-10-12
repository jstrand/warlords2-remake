
void __fpu_limit(void)

{
  undefined2 *puVar1;
  undefined2 *puVar2;
  undefined2 *puVar3;
  int in_stack_0000000a;
  int in_stack_0000000c;
  int in_stack_0000000e;
  int in_stack_00000010;
  uint in_stack_00000012;
  int in_stack_00000014;
  
  puVar1 = (undefined2 *)0x43fe;
  puVar3 = (undefined2 *)0x3bcd;
  if (in_stack_00000014 == 0) {
    puVar1 = (undefined2 *)0x407e;
    puVar3 = (undefined2 *)&DAT_451b_000a;
  }
  puVar2 = (undefined2 *)(in_stack_00000012 & 0x7fff);
  if (((puVar2 != (undefined2 *)0x7fff) && (puVar2 != puVar1)) &&
     ((puVar1 < puVar2 ||
      (((((puVar2 != (undefined2 *)0x0 || in_stack_00000010 != 0) || in_stack_0000000e != 0) ||
        in_stack_0000000c != 0) || in_stack_0000000a != 0 && (puVar2 < puVar3)))))) {
    DAT_4125_007f = 0x22;
  }
  return;
}


