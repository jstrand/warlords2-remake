
/* WARNING: Unable to track spacebase fully for stack */

undefined2 * __cdecl16far __F_SPUSH(void)

{
  undefined2 *puVar1;
  undefined2 *puVar2;
  int iVar3;
  undefined2 *in_AX;
  uint in_CX;
  uint uVar4;
  undefined2 in_DX;
  undefined2 unaff_DI;
  undefined2 *puVar5;
  undefined2 unaff_SS;
  undefined2 in_stack_00000000;
  undefined2 in_stack_00000002;
  undefined2 auStack_4 [2];
  
  iVar3 = -in_CX;
  *(undefined2 *)(&stack0x0002 + iVar3) = in_stack_00000002;
  *(undefined2 *)(&stack0x0000 + iVar3) = in_stack_00000000;
  *(undefined2 *)((int)auStack_4 + iVar3 + 2) = unaff_DI;
  *(undefined2 *)((int)auStack_4 + iVar3) = 0x4125;
  puVar5 = (undefined2 *)(&stack0x0004 + iVar3);
  for (uVar4 = in_CX >> 1; uVar4 != 0; uVar4 = uVar4 - 1) {
    puVar2 = puVar5;
    puVar5 = puVar5 + 1;
    puVar1 = in_AX;
    in_AX = in_AX + 1;
    *puVar2 = *puVar1;
  }
  for (uVar4 = (uint)((in_CX & 1) != 0); uVar4 != 0; uVar4 = uVar4 - 1) {
    puVar2 = puVar5;
    puVar5 = (undefined2 *)((int)puVar5 + 1);
    puVar1 = in_AX;
    in_AX = (undefined2 *)((int)in_AX + 1);
    *(undefined1 *)puVar2 = *(undefined1 *)puVar1;
  }
  return in_AX;
}


