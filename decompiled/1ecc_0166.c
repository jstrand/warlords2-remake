
undefined2 __cdecl16far auto_str_palloc_no_hdr(int param_1)

{
  long *plVar1;
  long *plVar2;
  long *plVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 uVar6;
  undefined2 uVar7;
  long *local_6;
  
  if (DAT_451b_2a3a == (long *)0x0) {
    uVar5 = 0;
  }
  else {
    uVar5 = (undefined2)((ulong)DAT_451b_2a3a >> 0x10);
    plVar3 = (long *)DAT_451b_2a3a;
    while( true ) {
      plVar2 = (long *)*plVar3;
      uVar5 = (undefined2)((ulong)plVar2 >> 0x10);
      plVar3 = (long *)plVar2;
      if ((*(char *)((int)plVar3 + 9) == '\0') && (*(int *)((int)plVar3 + 10) == param_1)) break;
      if (plVar2 == DAT_451b_2a3a) {
        FUN_251d_0099((char *)s_h_Apalloc__no_hdr_4125_33d3 + 3,0x4125);
      }
    }
    local_6 = plVar2;
    if ((*(char *)((int)plVar3[1] + 9) != '\0') &&
       (*(uint *)((int)plVar3[1] + 10) < *(uint *)((int)plVar3 + 10))) {
      local_6 = (long *)plVar3[1];
      uVar6 = (undefined2)((ulong)local_6 >> 0x10);
      plVar1 = (long *)local_6 + 3;
      *(int *)plVar1 = (int)*plVar1 + (int)plVar3[3];
      *local_6 = *plVar2;
      uVar7 = (undefined2)((ulong)*plVar2 >> 0x10);
      iVar4 = (int)*plVar2;
      *(undefined2 *)(iVar4 + 6) = uVar6;
      *(undefined2 *)(iVar4 + 4) = (long *)local_6;
      FUN_1ecc_0337(plVar3,uVar5);
    }
    plVar3 = (long *)local_6;
    uVar5 = (undefined2)((ulong)local_6 >> 0x10);
    if ((*(char *)((int)*local_6 + 9) != '\0') &&
       (*(uint *)((int)plVar3 + 10) < *(uint *)((int)*local_6 + 10))) {
      plVar2 = (long *)*local_6;
      uVar6 = (undefined2)((ulong)plVar2 >> 0x10);
      *(int *)(plVar3 + 3) = (int)plVar3[3] + (int)((long *)plVar2)[3];
      *local_6 = *plVar2;
      uVar7 = (undefined2)((ulong)*plVar2 >> 0x10);
      iVar4 = (int)*plVar2;
      *(undefined2 *)(iVar4 + 6) = uVar5;
      *(undefined2 *)(iVar4 + 4) = plVar3;
      FUN_1ecc_0337((long *)plVar2,uVar6);
    }
    *(undefined1 *)((int)plVar3 + 9) = 1;
    DAT_451b_2a3a = local_6;
    uVar5 = 1;
  }
  return uVar5;
}


