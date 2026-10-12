
undefined2 __cdecl16far FUN_1ecc_005c(uint param_1)

{
  long *plVar1;
  uint uVar2;
  undefined4 *puVar3;
  undefined2 in_DX;
  undefined4 *puVar4;
  int iVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  undefined4 *local_e;
  undefined4 local_a;
  
  if (DAT_451b_2a3a != (undefined4 *)0x0) {
    uVar2 = (param_1 >> 3) + 1;
    local_e = DAT_451b_2a3a;
    uVar6 = (undefined2)((ulong)DAT_451b_2a3a >> 0x10);
    puVar4 = (undefined4 *)DAT_451b_2a3a;
    do {
      plVar1 = (long *)*puVar4;
      uVar6 = (undefined2)((ulong)plVar1 >> 0x10);
      puVar4 = (undefined4 *)plVar1;
      if ((*(char *)((int)puVar4 + 9) != '\0') && (uVar2 <= *(uint *)(puVar4 + 3))) {
        local_a = plVar1;
        if (8 < *(int *)(puVar4 + 3) - uVar2) {
          puVar3 = (undefined4 *)auto_str_palloc_out_of_hdrs();
          local_a = (long *)CONCAT22(in_DX,puVar3);
          uVar7 = (undefined2)((ulong)*plVar1 >> 0x10);
          iVar5 = (int)*plVar1;
          *(undefined2 *)(iVar5 + 6) = in_DX;
          *(undefined2 *)(iVar5 + 4) = puVar3;
          *local_a = *plVar1;
          puVar3[1] = plVar1;
          *plVar1 = (long)local_a;
          *(uint *)(puVar4 + 3) = *(int *)(puVar4 + 3) - uVar2;
          *(uint *)(puVar3 + 3) = uVar2;
          *(int *)((int)puVar3 + 10) = *(int *)((int)puVar4 + 10) + *(int *)(puVar4 + 3);
        }
        uVar6 = (undefined2)((ulong)local_a >> 0x10);
        *(undefined1 *)((int)(undefined4 *)local_a + 9) = 0;
        DAT_451b_2a3a = local_e;
        return *(undefined2 *)((int)(undefined4 *)local_a + 10);
      }
      if (*(char *)((int)puVar4 + 9) != '\0') {
        local_e = plVar1;
      }
    } while (plVar1 != DAT_451b_2a3a);
  }
  return 0;
}


