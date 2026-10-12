
void __cdecl16far FUN_2372_1286(void)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  undefined2 uVar7;
  undefined4 local_a;
  ulong local_6;
  
  iVar2 = ((int *)DAT_451b_2fc8)[4];
  while (iVar2 != 0) {
    puVar3 = (undefined4 *)(*DAT_451b_2fc8 + (iVar2 + -1) * 0x2e);
    local_a = (undefined4 *)CONCAT22(((int *)DAT_451b_2fc8)[1],puVar3);
    local_6 = 0;
    iVar6 = 2;
    iVar1 = 0;
    while (iVar4 = iVar1, uVar7 = (undefined2)((ulong)*local_a >> 0x10),
          piVar5 = (int *)((int)*local_a + iVar4 * 4), *piVar5 != 0 || piVar5[1] != 0) {
      if (*(char *)*(undefined4 *)((int)*local_a + iVar4 * 4) == '%') {
        iVar1 = iVar4 + 1;
      }
      else {
        local_6 = local_6 | (long)iVar6;
        iVar6 = iVar6 << 1;
        iVar1 = iVar4 + 1;
        if (*(char *)*(undefined4 *)((int)*local_a + (iVar4 + 1) * 4) == '&') {
          iVar1 = iVar4 + 3;
        }
      }
    }
    *(ulong *)((int)puVar3 + 0x1a) = local_6 & 0xfffffe;
    iVar2 = iVar2 + -1;
  }
  return;
}


