
void __cdecl16far ai_data_init(int param_1)

{
  byte *pbVar1;
  undefined2 *puVar2;
  undefined1 uVar3;
  int iVar4;
  int iVar5;
  undefined2 *puVar6;
  undefined2 uVar7;
  
  puVar2 = DAT_451b_018e;
  uVar7 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  puVar6 = (undefined2 *)DAT_451b_018e;
  puVar6[0x25] = 0;
  *puVar2 = 0;
  puVar6[0x10] = 10;
  puVar6[0x11] = 0;
  puVar6[0x12] = 0;
  puVar6[6] = 0;
  puVar6[0xb] = 0;
  puVar6[0xc] = 0;
  puVar6[0xd] = 0;
  puVar6[0xe] = 0;
  puVar6[0xf] = 0;
  puVar6[0x26] = 0;
  puVar6[0x13] = 0;
  puVar6[0x14] = 0;
  puVar6[0x15] = 0;
  puVar6[0x16] = 0;
  puVar6[0x17] = 0;
  puVar6[0x18] = 0;
  puVar6[0x19] = 0;
  puVar6[0x1a] = 0;
  puVar6[0x1b] = 0;
  iVar5 = city_count;
  while (iVar4 = iVar5, iVar4 != 0) {
    if (opt_quick_start == 0) {
      uVar3 = 0;
    }
    else {
      uVar3 = 4;
    }
    uVar7 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    *(undefined1 *)((int)(undefined2 *)DAT_451b_018e + iVar4 + 0x55) = uVar3;
    *(undefined1 *)((int)(undefined2 *)DAT_451b_018e + iVar4 + 0xb9) = 0;
    *(undefined1 *)((int)(undefined2 *)DAT_451b_018e + iVar4 + 0x11d) = 0;
    iVar5 = iVar4 + -1;
    if (opt_hidden_map != 0) {
      pbVar1 = (byte *)((int)(undefined2 *)DAT_451b_018e + iVar4 + 0x11d);
      *pbVar1 = *pbVar1 | 1;
    }
  }
  if (*(int *)(param_1 * 2 + 0xd0) != 0) {
    iVar5 = *(int *)(param_1 * 2 + 0xc0);
    if (iVar5 == 0) {
      uVar7 = 0;
      goto code_r0x0005a55d;
    }
    if (iVar5 == 1) {
      uVar7 = 1;
      goto code_r0x0005a55d;
    }
  }
  uVar7 = 2;
code_r0x0005a55d:
  FUN_59bf_0d7b(uVar7,param_1);
  iVar5 = 4;
  while (iVar5 != 0) {
    ((undefined2 *)DAT_451b_018e)[(iVar5 + -1) * 0x2e + 0x126] = 0;
    iVar5 = iVar5 + -1;
  }
  iVar5 = 8;
  while (iVar5 != 0) {
    uVar7 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    puVar6 = (undefined2 *)DAT_451b_018e;
    puVar6[iVar5 + 0x20d] = 0;
    puVar6[iVar5 + 0x205] = 0;
    puVar6[iVar5 + 0x1fd] = 0;
    puVar6[iVar5 + 0x1f5] = 0;
    puVar6[iVar5 + 0x1ed] = 0;
    puVar6[iVar5 + 0x1e5] = 0;
    puVar6[iVar5 + 0x1dd] = 0;
    iVar5 = iVar5 + -1;
  }
  return;
}


