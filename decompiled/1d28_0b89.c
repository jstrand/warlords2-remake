
int __cdecl16near FUN_1d28_0b89(void)

{
  undefined2 *puVar1;
  uint uVar2;
  undefined2 uVar3;
  int in_AX;
  uint uVar4;
  int unaff_SI;
  undefined2 *puVar5;
  
  *(int *)(unaff_SI + 0xe) = in_AX;
  uVar2 = *(uint *)(unaff_SI + 10);
  uVar3 = *(undefined2 *)(unaff_SI + 0xe);
  puVar5 = (undefined2 *)0x0;
  for (uVar4 = *(uint *)(unaff_SI + 10) >> 1; uVar4 != 0; uVar4 = uVar4 - 1) {
    puVar1 = puVar5;
    puVar5 = puVar5 + 1;
    *puVar1 = 0xffff;
  }
  return (uVar2 >> 4) + in_AX;
}


