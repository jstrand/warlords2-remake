
void __cdecl16far FUN_5f19_0b72(int param_1)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  undefined2 uVar9;
  undefined2 unaff_SS;
  int local_24;
  int local_22;
  undefined2 local_1c [4];
  undefined2 local_14 [4];
  int local_c [4];
  int local_4;
  
  uVar1 = DAT_451b_018e._2_2_;
  iVar4 = (int)DAT_451b_018e + param_1 * 0x5c + 0x252;
  if ((*(uint *)((int)DAT_451b_018e + param_1 * 0x5c + 0x2a6) & 0x10) == 0) {
    if ((*(uint *)((int)DAT_451b_018e + param_1 * 0x5c + 0x2a6) & 8) == 0) {
      local_22 = *(int *)((int)DAT_451b_018e + param_1 * 0x5c + 0x2a4);
    }
    else {
      local_22 = 0x10;
    }
  }
  else {
    local_22 = 0xc;
  }
  iVar8 = 4;
  while (iVar6 = iVar8 + -1, iVar7 = city_count, iVar8 != 0) {
    local_1c[iVar6] = 0xffff;
    local_14[iVar6] = 0xffff;
    local_c[iVar6] = -1;
    *(undefined2 *)(iVar4 + iVar6 * 2) = 0xffff;
    iVar8 = iVar6;
  }
  while (iVar8 = iVar7 + -1, iVar7 != 0) {
    iVar7 = iVar8;
    if ((*(char *)(iVar8 * 0x41 + 0x1592) == current_player) &&
       (iVar6 = FUN_5f19_0a7a(iVar8,0), iVar6 != 0)) {
      uVar9 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
      iVar6 = (int)DAT_451b_018e;
      if ((((((*(uint *)(iVar6 + param_1 * 0x5c + 0x2a6) & 0x10) == 0) ||
            ((*(byte *)(iVar6 + iVar8 + 0x11e) & 8) != 0)) &&
           (((*(uint *)(iVar6 + param_1 * 0x5c + 0x2a6) & 8) == 0 ||
            ((*(byte *)(iVar6 + iVar8 + 0x11e) & 0x80) != 0)))) &&
          (((*(uint *)(iVar6 + param_1 * 0x5c + 0x2a6) & 0x20) == 0 ||
           ((*(byte *)(iVar6 + iVar8 + 0x11e) & 0x40) != 0)))) &&
         (((((*(uint *)(iVar6 + param_1 * 0x5c + 0x2a6) & 0x38) != 0 ||
            ((*(byte *)(iVar6 + iVar8 + 0x11e) & 200) == 0)) &&
           (iVar6 = FUN_623c_1a4d(iVar8,&local_4), iVar6 != 0)) &&
          (local_22 <= *(char *)(iVar8 * 0x41 + local_4 + 0x159f))))) {
        iVar6 = -1;
        local_24 = 100;
        iVar3 = 0;
        while ((iVar2 = iVar6, iVar3 < 4 && (iVar2 = iVar3, *(int *)(iVar4 + iVar3 * 2) != -1))) {
          if (local_c[iVar3] < local_24) {
            local_24 = local_c[iVar3];
            iVar6 = iVar3;
          }
          iVar3 = iVar3 + 1;
        }
        if ((iVar2 != -1) &&
           ((*(int *)(iVar4 + iVar2 * 2) == -1 ||
            (local_24 <= *(char *)(iVar8 * 0x41 + local_4 + 0x159b))))) {
          *(int *)(iVar4 + iVar2 * 2) = iVar8;
          local_c[iVar2] = (int)*(char *)(iVar8 * 0x41 + local_4 + 0x159b);
        }
      }
    }
  }
  for (iVar8 = 0; iVar8 < 4; iVar8 = iVar8 + 1) {
    piVar5 = (int *)(iVar4 + iVar8 * 2);
    if (*piVar5 != -1) {
      *(undefined1 *)((int)DAT_451b_018e + *piVar5 + 0x56) = 6;
    }
  }
  return;
}


