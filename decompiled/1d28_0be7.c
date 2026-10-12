
void __cdecl16near FUN_1d28_0be7(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  int iVar3;
  uint in_CX;
  uint uVar4;
  int iVar5;
  int in_DX;
  undefined4 *unaff_SI;
  undefined4 *puVar6;
  undefined4 *unaff_DI;
  undefined4 *puVar7;
  int unaff_ES;
  
  iVar3 = 0x4125;
  for (; (in_DX != 0 || (0x7ff8 < in_CX)); in_CX = in_CX + 0x8008) {
    puVar6 = unaff_SI;
    puVar7 = unaff_DI;
    for (iVar5 = 0x3ffc; iVar5 != 0; iVar5 = iVar5 + -1) {
      puVar2 = puVar7;
      puVar7 = puVar7 + 1;
      puVar1 = puVar6;
      puVar6 = puVar6 + 1;
      *puVar2 = *puVar1;
    }
    iVar3 = iVar3 + 0xfff;
    unaff_ES = unaff_ES + 0xfff;
    in_DX = in_DX - (uint)(in_CX < 0x7ff8);
  }
  if ((in_CX & 1) == 0) {
    for (uVar4 = in_CX >> 1 | (uint)((in_CX & 1) != 0) << 0xf; uVar4 != 0; uVar4 = uVar4 - 1) {
      puVar2 = unaff_DI;
      unaff_DI = unaff_DI + 1;
      puVar1 = unaff_SI;
      unaff_SI = unaff_SI + 1;
      *puVar2 = *puVar1;
    }
  }
  else {
    for (; in_CX != 0; in_CX = in_CX - 1) {
      puVar2 = unaff_DI;
      unaff_DI = (undefined4 *)((int)unaff_DI + 2);
      puVar1 = unaff_SI;
      unaff_SI = (undefined4 *)((int)unaff_SI + 2);
      *(undefined2 *)puVar2 = *(undefined2 *)puVar1;
    }
  }
  return;
}


