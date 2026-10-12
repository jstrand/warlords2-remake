
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far auto_ui_army_info(int param_1,undefined2 param_2)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  undefined2 unaff_SS;
  char *pcVar4;
  char *pcVar5;
  int local_1a [8];
  undefined2 local_a;
  undefined2 uStack_8;
  int local_6;
  int local_4;
  
  if (_LAB_4125_1174 == 0) {
    iVar1 = (param_1 + -0x130) / 0x20;
    if (iVar1 < 0) {
      iVar1 = 0;
    }
    if (7 < iVar1) {
      iVar1 = 7;
    }
    armies_at_tile(*(int *)(((undefined2 *)&DAT_451b_04ae)[DAT_4125_0d6a] * 0x16 + -0x7780),
                   *(int *)(((undefined2 *)&DAT_451b_04ae)[DAT_4125_0d6a] * 0x16 + -0x777e),
                   (int *)CONCAT22(unaff_SS,&local_4));
    if (iVar1 < local_4) {
      local_6 = 1;
      for (iVar2 = 0; iVar2 < local_4; iVar2 = iVar2 + 1) {
        if ((((undefined2 *)&DAT_451b_1ec0)[iVar2 * 2] == 0x2e1d) &&
           (((undefined2 *)&DAT_451b_04ae)[DAT_4125_0d6a] * 0x16 + -0x7780 ==
            ((undefined2 *)&DAT_451b_1ebe)[iVar2 * 2])) {
          local_1a[iVar2] = 0;
        }
        else {
          local_1a[iVar2] = local_6;
          local_6 = local_6 + 1;
        }
      }
      for (iVar2 = 0; iVar2 < local_4; iVar2 = iVar2 + 1) {
        if (local_1a[iVar2] == iVar1) {
          _local_a = CONCAT22(((undefined2 *)&DAT_451b_1ec0)[iVar2 * 2],
                              ((undefined2 *)&DAT_451b_1ebe)[iVar2 * 2]);
          break;
        }
      }
      uVar3 = (undefined2)((ulong)_local_a >> 0x10);
      iVar1 = (int)_local_a;
      ui_army_info((int)*(char *)(iVar1 + 4),(int)*(char *)(iVar1 + 6),(int)*(char *)(iVar1 + 8),
                   (int)*(char *)(iVar1 + 0xb),(int)*(char *)(iVar1 + 7),(int)*(char *)(iVar1 + 10),
                   param_1,param_2);
    }
    else {
      FUN_740d_1201();
      pcVar4 = get_string(0x84,1);
      pcVar5 = get_string(0x84,0);
      FUN_740d_10e1((char *)pcVar5,(int)((ulong)pcVar5 >> 0x10),pcVar4);
    }
    _LAB_4125_1174 = 1;
  }
  return;
}


