
undefined2 * __cdecl16far FUN_21b5_01f4(undefined2 *param_1,undefined2 param_2,undefined4 param_3)

{
  clock_t *pcVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  undefined2 in_DX;
  undefined2 *puVar10;
  clock_t *unaff_SI;
  undefined2 uVar11;
  
  *param_1 = param_2;
  uVar11 = (undefined2)((ulong)param_1 >> 0x10);
  puVar10 = (undefined2 *)param_1;
  *(undefined4 *)(puVar10 + 1) = param_3;
  pcVar1 = clock(unaff_SI);
  *(clock_t **)(puVar10 + 3) = (clock_t *)CONCAT22(in_DX,pcVar1);
  FUN_22bf_0af4(puVar10 + 5);
  uVar2 = FUN_22bf_0944();
  uVar3 = bioskey(2);
  if ((uVar2 & 1) == 0) {
    uVar4 = 0;
  }
  else {
    uVar4 = 0x80;
  }
  if ((uVar2 & 2) == 0) {
    uVar2 = 0;
  }
  else {
    uVar2 = 0x40;
  }
  if ((uVar3 & 3) == 0) {
    uVar5 = 0;
  }
  else {
    uVar5 = 0x200;
  }
  if ((uVar3 & 4) == 0) {
    uVar6 = 0;
  }
  else {
    uVar6 = 0x100;
  }
  if ((uVar3 & 8) == 0) {
    uVar7 = 0;
  }
  else {
    uVar7 = 0x800;
  }
  if ((uVar3 & 0x10) == 0) {
    uVar8 = 0;
  }
  else {
    uVar8 = 0x1000;
  }
  if ((uVar3 & 0x20) == 0) {
    uVar9 = 0;
  }
  else {
    uVar9 = 0x2000;
  }
  if ((uVar3 & 0x40) == 0) {
    uVar3 = 0;
  }
  else {
    uVar3 = 0x400;
  }
  puVar10[7] = uVar4 | uVar2 | uVar5 | uVar6 | uVar7 | uVar8 | uVar9 | uVar3;
  return puVar10;
}


