
int __cdecl16far FUN_5ad0_0f3c(int param_1)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  undefined2 in_DX;
  int iVar4;
  int iVar5;
  int local_10;
  int local_e;
  int local_a;
  int local_8;
  undefined2 local_6;
  undefined2 uStack_4;
  
  uVar1 = FUN_1555_1b0a();
  _local_6 = CONCAT22(in_DX,uVar1);
  local_e = -1;
  local_10 = 1000;
  iVar4 = city_count;
LAB_5ad0_10cd:
  do {
    do {
      iVar5 = iVar4 + -1;
      if (iVar4 == 0) {
        FUN_1555_1b2e();
        return local_e;
      }
      iVar4 = iVar5;
    } while (*(char *)(*(byte *)(*(int *)(iVar5 * 0x41 + 0x157f) * 0xe0 +
                                *(int *)(iVar5 * 0x41 + 0x157d) * 2) + 0x710) != '\n');
    uVar1 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  } while (((((*(byte *)((int)DAT_451b_018e + iVar5 + 0x11e) & 1) != 0) ||
            (*(int *)((int)DAT_451b_018e + 0x46) == iVar5)) ||
           ((*(char *)(iVar5 * 0x41 + 0x1592) != '\x0f' &&
            ((*(char *)(iVar5 * 0x41 + 0x1592) != current_player &&
             ((*(byte *)(current_player * 8 + (int)*(char *)(iVar5 * 0x41 + 0x1592) + 0x153b) & 3)
              != 2)))))) || (iVar2 = FUN_59bf_0a85(iVar5,&local_a), local_a == -1));
  if (*(char *)(iVar5 * 0x41 + 0x1592) != current_player) goto code_r0x0005bd71;
  if (*(char *)((int)DAT_451b_018e + iVar5 + 0x56) == '\a') {
    iVar3 = 10;
  }
  else {
    iVar3 = 0x1e;
  }
  iVar2 = iVar2 + iVar3;
  if (3 < param_1) {
    iVar2 = iVar2 + 100;
  }
  goto LAB_5ad0_10c2;
code_r0x0005bd71:
  local_8 = FUN_623c_15c5();
  if (0x4a < local_8) {
LAB_5ad0_10c2:
    if (iVar2 < local_10) {
      local_10 = iVar2;
      local_e = iVar5;
    }
  }
  goto LAB_5ad0_10cd;
}


