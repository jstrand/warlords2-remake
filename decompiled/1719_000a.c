
void __cdecl16near FUN_1719_000a(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  code *pcVar3;
  uint uVar4;
  int unaff_BP;
  int unaff_SI;
  undefined4 *puVar5;
  undefined4 *puVar6;
  undefined2 uVar7;
  undefined2 unaff_SS;
  undefined2 uVar8;
  
  if (*(int *)(unaff_BP + 0x10) <= unaff_SI) {
    uVar7 = (undefined2)((ulong)*(undefined4 *)(unaff_BP + 0xc) >> 0x10);
    puVar6 = (undefined4 *)(undefined4 *)*(undefined4 *)(unaff_BP + 0xc);
    uVar8 = (undefined2)((ulong)*(undefined4 *)(unaff_BP + 0x10) >> 0x10);
    puVar5 = (undefined4 *)(undefined4 *)*(undefined4 *)(unaff_BP + 0x10);
    uVar4 = *(uint *)(unaff_BP + 0x14);
    if ((uVar4 & 3) == 0) {
      for (uVar4 = uVar4 >> 2 | uVar4 << 0xe; uVar4 != 0; uVar4 = uVar4 - 1) {
        puVar2 = puVar6;
        puVar6 = puVar6 + 1;
        puVar1 = puVar5;
        puVar5 = puVar5 + 1;
        *puVar2 = *puVar1;
      }
    }
    else if ((uVar4 & 1) == 0) {
      for (uVar4 = uVar4 >> 1 | (uint)((uVar4 & 1) != 0) << 0xf; uVar4 != 0; uVar4 = uVar4 - 1) {
        puVar2 = puVar6;
        puVar6 = (undefined4 *)((int)puVar6 + 2);
        puVar1 = puVar5;
        puVar5 = (undefined4 *)((int)puVar5 + 2);
        *(undefined2 *)puVar2 = *(undefined2 *)puVar1;
      }
    }
    else {
      for (; uVar4 != 0; uVar4 = uVar4 - 1) {
        puVar2 = puVar6;
        puVar6 = (undefined4 *)((int)puVar6 + 1);
        puVar1 = puVar5;
        puVar5 = (undefined4 *)((int)puVar5 + 1);
        *(undefined1 *)puVar2 = *(undefined1 *)puVar1;
      }
    }
    pcVar3 = (code *)swi(0x21);
    (*pcVar3)();
    return;
  }
  return;
}


