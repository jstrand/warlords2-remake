
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_834b_0000(int param_1,int param_2,int *param_3)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  undefined2 unaff_SS;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 *puVar8;
  undefined4 uVar9;
  undefined2 uVar10;
  undefined2 uVar11;
  int local_12;
  int iStack_10;
  int local_e;
  int iStack_c;
  undefined4 local_a [2];
  
  if ((((opt_hidden_map != 0) && (*(int *)(current_player * 2 + 0xd0) != 0)) && (DAT_3c04_0159 == 0)
      ) && (DAT_4125_2d28 == 0)) {
    return;
  }
  if (((opt_hidden_map != 0) && (*(int *)(current_player * 2 + 0xd0) != 0)) &&
     ((DAT_3c04_0159 != 0 && ((DAT_4125_3254 != 0 && (param_3 != (int *)0x0)))))) {
    FUN_8961_0698();
    return;
  }
  piVar4 = (int *)param_3;
  uVar10 = (undefined2)((ulong)param_3 >> 0x10);
  if (((param_3 == (int *)0x0) || (8 < piVar4[2])) || (8 < piVar4[3])) {
    bVar1 = false;
  }
  else {
    bVar1 = true;
  }
  if (DAT_4125_2d1e == '\0') {
    FUN_2012_09cf(0x2cea,0x4125);
    FUN_2012_01fe(0x2cea,0x4125,0x20,0x20);
  }
  if (param_3 == (int *)0x0) {
    __F_SCOPY(0x2a88,0x4125,&local_12,unaff_SS);
    uVar10 = 0x70;
    uVar11 = 0x9c;
    iVar2 = 0;
    iVar3 = 0;
    puVar8 = (undefined4 *)CONCAT22(unaff_SS,local_a);
  }
  else {
    __F_SCOPY(piVar4,uVar10,local_a,unaff_SS);
    iStack_10 = piVar4[1] << 1;
    iStack_c = piVar4[3] << 1;
    local_12 = (*param_3 << 1) / 0x10 << 4;
    local_e = ((piVar4[2] * 2 + -1) / 0x10 + 1) * 0x10;
    if (!bVar1) goto LAB_834b_02ea;
    __F_SCOPY(piVar4,uVar10,0x5eba,0x4125);
    __F_SCOPY(piVar4,uVar10,(undefined2 *)&DAT_451b_1f52,0x4125);
    DAT_451b_1f52 = (DAT_451b_1f52 >> 2) << 2;
    DAT_451b_1f56 = ((DAT_451b_1f56 + -1 >> 3) + 1) * 8;
    if (DAT_451b_1f52 < 0) {
      DAT_451b_1f52 = 0;
      DAT_451b_1f56 = 8;
    }
    if (DAT_451b_1f54 < 0) {
      DAT_451b_1f54 = 0;
      DAT_451b_1f58 = 8;
    }
    if (0x70 < DAT_451b_1f52 + DAT_451b_1f56) {
      DAT_451b_1f52 = 0x68;
      DAT_451b_1f56 = 8;
    }
    if (0x9c < DAT_451b_1f54 + DAT_451b_1f58) {
      DAT_451b_1f54 = 0x94;
      DAT_451b_1f58 = 8;
    }
    __F_SCOPY((undefined2 *)&DAT_451b_1f52,0x4125,(undefined2 *)&DAT_451b_1f4a,0x4125);
    DAT_451b_1f4a = DAT_451b_1f4a + -4;
    DAT_451b_1f4c = DAT_451b_1f4c + -4;
    DAT_451b_1f4e = DAT_451b_1f4e + 8;
    DAT_451b_1f50 = DAT_451b_1f50 + 8;
    if (DAT_451b_1f4a < 0) {
      DAT_451b_1f4a = 0;
    }
    if (DAT_451b_1f4c < 0) {
      DAT_451b_1f4c = 0;
    }
    if (0x70 < DAT_451b_1f4a + DAT_451b_1f4e) {
      DAT_451b_1f4a = 0x60;
      DAT_451b_1f4e = 0x10;
    }
    if (0x9c < DAT_451b_1f4c + DAT_451b_1f50) {
      DAT_451b_1f4c = 0x8c;
      DAT_451b_1f50 = 0x10;
    }
    FUN_216d_002e((undefined2 *)&DAT_451b_1f42,0x4125,DAT_451b_1f52 << 1,DAT_451b_1f54 << 1,
                  DAT_451b_1f56 << 1,DAT_451b_1f58 << 1);
    FUN_216d_002e((undefined2 *)&DAT_451b_1f3a,0x4125,DAT_451b_1f4a << 1,DAT_451b_1f4c << 1,
                  DAT_451b_1f4e << 1,DAT_451b_1f50 << 1);
    FUN_216d_000e((undefined4 *)&DAT_451b_1f36,0x4125,DAT_451b_1f42 + 400,DAT_451b_1f44 + 0x1e);
    FUN_216d_002e(0x5eca,0x4125,0,0,(int)DAT_451b_1f3e,(int)((ulong)DAT_451b_1f3e >> 0x10));
    uVar10 = (undefined2)DAT_451b_1f46;
    uVar11 = (undefined2)((ulong)DAT_451b_1f46 >> 0x10);
    iVar3 = DAT_451b_1f44 - _DAT_451b_1f3c;
    iVar2 = DAT_451b_1f42 - DAT_451b_1f3a;
    puVar8 = &DAT_451b_1f62;
  }
  FUN_216d_002e(puVar8,iVar2,iVar3,uVar10,uVar11);
LAB_834b_02ea:
  if (((param_1 == 0) || (DAT_4125_2d25 == 0)) || (param_2 != 0)) {
    FUN_22bf_07a5();
    FUN_1997_034a(7);
    FUN_1997_060a(7);
    if (opt_hidden_map == 0) {
      (*DAT_451b_2a74)();
    }
    else {
      if (bVar1) {
        FUN_24d0_0497();
        uVar9 = 0;
        uVar7 = 0x41252cea;
        uVar5 = _DAT_451b_1f3a;
        uVar6 = DAT_451b_1f3e;
        iVar3 = (int)_DAT_451b_1f3a;
        iVar2 = (int)((ulong)_DAT_451b_1f3a >> 0x10);
      }
      else {
        FUN_24d0_0497();
        uVar9 = CONCAT22(iStack_10,local_12);
        uVar7 = 0x41256795;
        uVar6 = CONCAT22(iStack_c,local_e);
        uVar5 = CONCAT22(iStack_10,local_12);
        iVar3 = local_12;
        iVar2 = iStack_10;
      }
      (*DAT_451b_2a68)(0x24d0,0x581d,0x4125,uVar5,uVar6,uVar7,uVar9,0x54e3,0x4125,iVar3,iVar2);
    }
    FUN_1997_06bb(7);
    if ((param_1 == 0) || ((DAT_4125_2d25 != 0 && (param_2 == 0)))) {
      DAT_4125_2d25 = 0;
    }
    else {
      FUN_834b_0ed7(local_a);
      DAT_4125_2d25 = 1;
    }
    iVar3 = FUN_54f6_08bf();
    if (iVar3 == 0) {
      if (bVar1) {
        FUN_8961_05ec();
      }
      else {
        FUN_8961_05ec();
      }
      (*DAT_451b_2a74)();
      FUN_8961_0698();
    }
    else {
      FUN_54f6_08c7();
      if ((param_3 == (int *)0x0) && (opt_hidden_map != 0)) {
        FUN_2012_0bb7(0x6795,0x4125,local_12,iStack_10,local_e,4);
        FUN_2012_0f08(0x6795,0x4125,local_12,iStack_10,iStack_c,4);
        FUN_2012_0bb7(0x6795,0x4125,local_12,iStack_10 + iStack_c + -1,local_e,4);
      }
      (*DAT_451b_2a74)();
    }
    FUN_22bf_0757();
    if (!bVar1) {
      FUN_8611_1927();
    }
  }
  return;
}


